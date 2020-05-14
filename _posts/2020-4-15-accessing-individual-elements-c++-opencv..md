---
layout: post
title: 'Retrieving elements from an array in OpenCV C++'
date: '2020-4-16'
categories: jekyll update
published: true
---

# Accessing Array Elements Individually

There are three common ways to access matrix elements in OpenCV C++. The `.at` template member function, the classical pointer access and iterators.


## Template member funtion `.at`

As the name says it is a member function and hence it is linked to the template and its instaniation type. Let's see an example. If we create an array of floats as in 

```
int size[] {10, 10, 10};
cv::Mat m(3, size, CV_32F);

m.at<float>(1, 2, 3);
```
we can access using `.at<data_type>(position_indexes)`, where the data type corresponds to the data returned. In this case, it was a 3D array of dimensions `[10, 10, 10]`. We can also use own OpenCV data types like `Vec3f`.

```
cv::Mat m1(10, 10, CV_32FC3);

m1.at<cv::Vec3f>(2, 2);
```

In this case we have created a 3D matrix of dimensions `[10, 10, 3]`, following the convention of `[H, W, C]`. To those two ways, more variations are available for accessing elements with add, for example, using `cv::Point_<>` and  a standard array.

```
cv::Mat m2(20, 20, CV_8UC3);

m2.at<cv::Vec3i>(cv::Point2i(10,10));

int size3[] {10, 10, 10, 10, 10};
cv::Mat m3(5,size3, CV_8UC3, cv::Scalar(0));

int idx[] {1, 2, 3, 4, 5};
m3.at<uchar>(idx);
```

## C-Style pointer access

A faster way [1] to access matrix elements is to use also the member function `.ptr` which works as a classic pointer style access, returning the pointer to the specific row.

One requirement to the usage of `.ptr` is that the array must be continuous. We can check that with the member function of `cv::Mat::iscontinuous()`

```
int size4[] {100, 100, 100};
cv::Mat m4(3, size4, CV_32F, cv::Scalar(0));

m4.isContinuous();

for ( std::size_t i = 0; i != m4.size[0]; i++)
{
    float* p = m4.ptr<float>(i);
    for (std::size_t j = 0; j != m4.size[1]*m4.size[2]; j++)
    {
        *(p + j);
    }
}
```

As the function `cv::Mat::ptr` returns the pointer to the row, we have to go through the other dimensions from that point running through pointers. Also, as in the case of `at`, the matrix element type will determine the pointer type. 

```
cv::Mat m5(100, 100, CV_32FC3, cv::Scalar(0));

m5.isContinuous();

for ( std::size_t i = 0; i != m4.size[0]; i++)
{
    cv::Vec3f* p = m5.ptr<cv::Vec3f>(i);
    for (std::size_t j = 0; j != m5.size[1]; j++)
    {
        *(p + j);
    }
}
```
## Iterators

The third way to access a matrix element is an iterator, which have an analogous functioning w.r.t STL containers. 

```
int size5[] {100, 100, 100};
cv::Mat m6(3, size5, CV_32FC3, cv::Scalar(0));

cv::MatConstIterator_<cv::Vec3f> it = m6.begin<cv::Vec3f>();

while( it != m6.end<cv::Vec3f>())
{
    *it
}
```

As seen, we use `cv::Mat::begin()`  and `cv::Mat::end()` functions to start and bound the iterator, which we instantiate with `cv::MatConstIterator`. The returned value by `*it` is obviously a `cv::Vec3f`. 

# Conclusion

We ahve seen three different ways of accessing individual elements in a `cv::Mat` array for C++ OpenCV. In next posts, we will see how to access elements in blocks and iterating through different arrays at the same time.
And remember [always use](https://stackoverflow.com/questions/3259413/should-you-always-use-int-for-numbers-in-c-even-if-they-are-non-negative/3261019#3261019) `std::size_t` when using sizes ()

[1] Depends on compiler optimization