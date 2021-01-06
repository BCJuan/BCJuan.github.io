---
layout: post
title: 'Simple classification app with CameraX, TFLite and Android TFLite support (Java)'
date: '2021-01-06'
categories: jekyll update
published: true
subtitle: >-
    How to use analyze method from CameraX altogether with the android tflite support library (Java)
use_math: true
---

# Introduction

I am novice to Android App development, however I have been developing neural network models for gesture recognition for microcontrollers for a while. I wanted then to try out to develop a simple classification app with Android.

In this blog post I will briefly describe the steps to build a simple application for classification using CameraX  and the Android TFLite support library, all with Java.

I have seen the massive shift that Google has taken towards Kotlin development, and it has been a little difficult for me to adapt and understand some of the code that I have stumbled upon, since  I was more interested in learning Java due to its broader scope of application. Anyway, most Kotlin code I've seen can be adapted to Java without great difficulty.

As a teaser, you can see the end of the demo here:

![](https://github.com/BCJuan/SimpleClassificationApp/blob/master/demo.gif)


The full code is at: ![]
The post will consist of two parts:

1. CameraX simple backbone
2. Classification procedure
    1. Model and label loading
    2. Image transformation
    3. Classification

## CameraX backbone

I have read that the CameraX API has ![simplified the work of developers](https://developer.android.com/jetpack/androidx/releases/camera?hl=ru)  compared to Camera 2, but I do not know about that since I have not used Camera 2. What I can say is that using CameraX is not difficult. Let's see how we can do it.

### Layout

First, we define only two elements in a constrained layout: the camera preview, where the frames will be hold, and the text where the results from classification will be shown.

```
<androidx.constraintlayout.widget.ConstraintLayout
    xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:tools="http://schemas.android.com/tools"
    xmlns:app="http://schemas.android.com/apk/res-auto"
    android:layout_width="match_parent"
    android:layout_height="match_parent"
    tools:context=".MainActivity">

    <androidx.camera.view.PreviewView
        android:id="@+id/viewFinder"
        android:layout_width="match_parent"
        android:layout_height="match_parent"
        tools:layout_editor_absoluteX="0dp"
        tools:layout_editor_absoluteY="133dp">
    </androidx.camera.view.PreviewView>

    <TextView
        android:id="@+id/tvResults"
        android:layout_width="match_parent"
        android:layout_height="100dp"
        android:background="#FFFFFF"
        android:foregroundTint="@color/white"
        android:textColor="#0C0C0C"
        android:textColorHint="#0E0C0C"
        android:visibility="visible"
        app:layout_constraintBottom_toBottomOf="parent"
        app:layout_constraintLeft_toLeftOf="parent"
        app:layout_constraintRight_toRightOf="parent"
        tools:visibility="visible" />
</androidx.constraintlayout.widget.ConstraintLayout>
```

### Camera Permissions and Libraries

We need permissions for the camera. First, we add to the `AndroidManifest.xml` the appropriate support:

```
<uses-feature android:name="android.hardware.camera.any" />
<uses-permission android:name="android.permission.CAMERA" />
```

Then, in the main class we add the definitions for the request code and required permissions:

```
private int REQUEST_CODE_PERMISSIONS = 1001;
private final String [] REQUIRED_PERMISSIONS = new String []{Manifest.permission.CAMERA};
```

Then, on the `onCreate` method we start the camera, but, of course, checking before if we have the permissions:

```
if (allPermissionsGranted()){
    startCamera();
}
else{
    ActivityCompat.requestPermissions(this, REQUIRED_PERMISSIONS,
            REQUEST_CODE_PERMISSIONS);
}
```

where we have the function `allPermissionsGranted`, which is just a checker of permissions, and the function `onRequestPermissionsResult`, which is activated after the user interaction with the permission UI and repeats the loop of permission check again:

```
private boolean allPermissionsGranted(){
    for(String permission: REQUIRED_PERMISSIONS){
        if(ContextCompat.checkSelfPermission(
                this, permission) != PackageManager.PERMISSION_GRANTED){
            return false;
        }
    }
    return true;
}

@Override
public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                        @NonNull int[] grantResults) {
    if (requestCode == REQUEST_CODE_PERMISSIONS){
        if(allPermissionsGranted()){
            startCamera();
        }
        else{
            Toast.makeText(this, "Please, give access to camera",
                    Toast.LENGTH_SHORT).show();
            this.finish();
        }
    }
}
```
Also, we need to add the appropriate libraries to the `.gradle` file:

```
dependencies{
    ...
    implementation "androidx.camera:camera-camera2:$camerax_version"
    // CameraX Lifecycle Library
    implementation "androidx.camera:camera-lifecycle:$camerax_version"
    // CameraX View class
    implementation "androidx.camera:camera-view:1.0.0-alpha14"
    ...
}
```

### Camera

First, in the function `startCamera`, we instantiate a `ProcessCameraProvider`, which returns a `ListenableFuture` that will be linked to the lifecycle of parent (meaning we do not have to close and open the camera manually). After that, we add a listener with a runnable and the executor of the main thread. In the runnable we add the `ProcessCameraProvider` and proceed by binding the preview and other actions:

```
private void startCamera(){
    ListenableFuture<ProcessCameraProvider>
            cameraProviderFuture = ProcessCameraProvider.getInstance(this);
    cameraProviderFuture.addListener(
            new Runnable() {
                @Override
                public void run() {
                    try {
                        ProcessCameraProvider cameraProvider = cameraProviderFuture.get();
                        bindPreview(cameraProvider);
                    } catch (ExecutionException | InterruptedException e) { }
                }
            },
            ActivityCompat.getMainExecutor(this)
    );
}
```

In the `bindPreview` method we four different things: 

1. Link surface preview (to link the camera to our preview were frames will be visualized)

```
Preview preview = new Preview.Builder().build();
preview.setSurfaceProvider(mPreviewView.createSurfaceProvider());
```

2. Establish camera selector and define its properties
```
CameraSelector cameraSelector = new CameraSelector.Builder().requireLensFacing(
        CameraSelector.LENS_FACING_BACK).build();
```

3. Establish Image analysis and related tasks, in this case, classification (explained later). If you want to test the app without classification (only showing the camera preview) you can delete the lines regarding `classifier` and `tvResults`.
```
ImageAnalysis imageAnalysis =
        new ImageAnalysis.Builder()
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build();

imageAnalysis.setAnalyzer(ActivityCompat.getMainExecutor(this),
        new ImageAnalysis.Analyzer() {
            @Override
            public void analyze(@NonNull ImageProxy image) {
                String result;
                result = classifier.classify(image);
                tvResults.setText(result);
                image.close();
            }
        });
```     

4. Finally, unibind everything just in case, and bind the lifecycle

```
cameraProvider.unbindAll();
cameraProvider.bindToLifecycle(this, cameraSelector,
        preview, imageAnalysis);
```

With this we have everything set up with regards to the camera operability. As you may have seen it is really simple, and we do not use any further functionalities like `CameraCapture` with callbacks, for example. We are already set to classify with the analyze method, which will be run on every frame.

## Classification

We are going to use a quantized MobileNetV1 model reduced to a 1/4 of its size and using 128x128 images. See the ![model link at TF Hub](https://tfhub.dev/tensorflow/lite-model/mobilenet_v1_0.25_128_quantized/1/metadata/1). It classifies ImageNet classes, so we are also going to need the labels.

To add the model you can follow the instructions at ![Android quickstart guide](https://www.tensorflow.org/lite/guide/android) to add the model through ML Binding. This mainly means that you right-click on the Android left panel `res` and add a new file of the type `Other`->`Tensorflow Lite Model`. And that is it. In the case of the labels, you just need to add the `labels.txt` as an asset file by right-click in the android panel -> `New` -> `Assets folder` and creating there a file where the classes are stored.

As seen in the last section, in the image analysis we have called a `classifier.classify`. This comes from a class which holds everything realted to image processing and classification. Let's see:

```
public class Classifier {

    private Context context;
    Interpreter tflite;
    final String ASSOCIATED_AXIS_LABELS = "labels.txt";
    List<String> associatedAxisLabels = null;

    public Classifier(Context context)
    {
        this.context = context;

        try {
            associatedAxisLabels = FileUtil.loadLabels(context, ASSOCIATED_AXIS_LABELS);
        } catch (IOException e) {
            Log.e("tfliteSupport", "Error reading label file", e);
        }

        try{
            MappedByteBuffer tfliteModel
                    = FileUtil.loadMappedFile(context,
                    "mobilenet_v1_0.25_128_quantized_1_metadata_1.tflite");
            tflite = new Interpreter(tfliteModel);
        } catch (IOException e){
            Log.e("tfliteSupport", "Error reading model", e);
        }
    }
    ...
}
```

As you can see in the constructor of the class, first we load the labels intos a `List<String>` and then the model into an interpreter. Then the other important method is the classification:

```
public class Classifier {
    ...
    public String classify(ImageProxy image){
        @SuppressLint("UnsafeExperimentalUsageError")
        Image img = image.getImage();
        Bitmap bitmap = Utils.toBitmap(img);
        int rotation = Utils.getImageRotation(image);
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();

        int size = height > width ? width : height;
        ImageProcessor imageProcessor = new ImageProcessor.Builder()
                .add(new ResizeWithCropOrPadOp(size, size))
                .add(new ResizeOp(128, 128, ResizeOp.ResizeMethod.BILINEAR))
                .add(new Rot90Op(rotation))
                .build();
        TensorImage tensorImage = new TensorImage(DataType.UINT8);
        tensorImage.load(bitmap);
        tensorImage = imageProcessor.process(tensorImage);
        TensorBuffer probabilityBuffer =
                TensorBuffer.createFixedSize(new int[]{1, 1001}, DataType.UINT8);
        if(null != tflite) {
            tflite.run(tensorImage.getBuffer(), probabilityBuffer.getBuffer());
        }
        TensorProcessor probabilityProcessor =
                new TensorProcessor.Builder().add(new NormalizeOp(0, 255)).build();

        String result = " ";
        if (null != associatedAxisLabels) {
            // Map of labels and their corresponding probability
            TensorLabel labels = new TensorLabel(associatedAxisLabels,
                    probabilityProcessor.process(probabilityBuffer));

            // Create a map to access the result based on label
            Map<String, Float> floatMap = labels.getMapWithFloatValue();
            result = Utils.writeResults(floatMap);
        }
        return result;
    }
}
```

The analyze method returns an `ImageProxy` which first we convert to a Bitmap, and several infor about it.

```

Image img = image.getImage();
Bitmap bitmap = Utils.toBitmap(img);
int rotation = Utils.getImageRotation(image);
int width = bitmap.getWidth();
int height = bitmap.getHeight();
int size = height > width ? width : height;
```

You can find the `ImageProxy` to Bitmap conversion in `Utils.java`:

```
public static Bitmap toBitmap(Image image) {
    Image.Plane[] planes = image.getPlanes();
    ByteBuffer yBuffer = planes[0].getBuffer();
    ByteBuffer uBuffer = planes[1].getBuffer();
    ByteBuffer vBuffer = planes[2].getBuffer();

    int ySize = yBuffer.remaining();
    int uSize = uBuffer.remaining();
    int vSize = vBuffer.remaining();

    byte[] nv21 = new byte[ySize + uSize + vSize];
    //U and V are swapped
    yBuffer.get(nv21, 0, ySize);
    vBuffer.get(nv21, ySize, vSize);
    uBuffer.get(nv21, ySize + vSize, uSize);

    YuvImage yuvImage = new YuvImage(nv21, ImageFormat.NV21, image.getWidth(), image.getHeight(), null);
    ByteArrayOutputStream out = new ByteArrayOutputStream();
    yuvImage.compressToJpeg(new Rect(0, 0, yuvImage.getWidth(), yuvImage.getHeight()), 100, out);

    byte[] imageBytes = out.toByteArray();
    return BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
}
```

Then what we do is to define the `ImageProcessor` that will hold the sequence of transforms to the input in order to be able to feed it to the model.

```
ImageProcessor imageProcessor = new ImageProcessor.Builder()
        .add(new ResizeWithCropOrPadOp(size, size))
        .add(new ResizeOp(128, 128, ResizeOp.ResizeMethod.BILINEAR))
        .add(new Rot90Op(rotation))
        .build();
```

In this case, we have croped the image to an square one to be able to resize it after preserving proportions and rotate if it is needed. Next, we create a `TensorImage` to hold the input image and load the Bitmap to it. Also, we create the vector of classes where the prediction of the model will be stored:

```
TensorImage tensorImage = new TensorImage(DataType.UINT8);
tensorImage.load(bitmap);
tensorImage = imageProcessor.process(tensorImage);
TensorBuffer probabilityBuffer =
        TensorBuffer.createFixedSize(new int[]{1, 1001}, DataType.UINT8);
```

Finally, we run the model, get the vector of probabilities and then normalize it:

```
if(null != tflite) {
    tflite.run(tensorImage.getBuffer(), probabilityBuffer.getBuffer());
}
TensorProcessor probabilityProcessor =
        new TensorProcessor.Builder().add(new NormalizeOp(0, 255)).build();
```
Finally, we only need to link the greatest values on the probability buffer to its classes and set it as text in the `TextView`
```
String result = " ";
if (null != associatedAxisLabels) {
    // Map of labels and their corresponding probability
    TensorLabel labels = new TensorLabel(associatedAxisLabels,
            probabilityProcessor.process(probabilityBuffer));

    // Create a map to access the result based on label
    Map<String, Float> floatMap = labels.getMapWithFloatValue();
    result = Utils.writeResults(floatMap);
}
return result;
```

*UPDATE*: I will add soon a section for using a delegate such as GPU.

# Conclusion

In this post I have shown how to build a simple Android app to classify objects. It makes use of CameraX Java API and TF Lite support library for androud, and show examples of some of the functionalities of both libraries. FOr example, we have seen the `Analyzer` method of CameraX and the `ImageProcessor` from TFLite Android Support Library, among other features. 

Hope you like it. In next posts, we will try to build more complicated apps with purposes other than classification.

The full code is available at ![BCJuan/SimpleClassificationApp](https://github.com/BCJuan/SimpleClassificationApp).

# References used to develop this app

+ ![https://codelabs.developers.google.com/codelabs/camerax-getting-started/index.html?index=..%2F..index#1](https://codelabs.developers.google.com/codelabs/camerax-getting-started/index.html?index=..%2F..index#1)
+ ![https://akhilbattula.medium.com/android-camerax-java-example-aeee884f9102](https://akhilbattula.medium.com/android-camerax-java-example-aeee884f9102)
+ ![https://stackoverflow.com/questions/56772967/converting-imageproxy-to-bitmap](https://stackoverflow.com/questions/56772967/converting-imageproxy-to-bitmap)
+ ![https://www.tensorflow.org/lite/guide/android](https://www.tensorflow.org/lite/guide/android)
+ ![https://www.tensorflow.org/lite/inference_with_metadata/lite_support](https://www.tensorflow.org/lite/inference_with_metadata/lite_support)
