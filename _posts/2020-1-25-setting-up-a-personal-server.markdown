# Setting up a personal GPU server. Part 1

In this post I want to explain a little bit my vicissitudes in setting up my desktop computer as a _server_ for GPU computation. Of course, I assume you already have your GPUs working on your computer.  The idea is also to show the steps that I followed in a progressive manner: from VNC in the local network to a ssh accesible computer across the net. Unfortunately for windows users, this guide/story is only for Ubuntu based computers.

## Local VNC

[VNC](https://en.wikipedia.org/wiki/Virtual_Network_Computing) stands for Virtual Network Computing and allows for taking remote control of computer. Basically, it allows to share the graphical desktop. One strength is that it is platform independent. The main weakneses are its graphical nature and its limitation for passwords. The first does not make it useful in poor connection environments and the second, with its limitation of 8 characters for passwords, make it somehow insecure to use in an open network or across the internet.

The set up is easy. First open the _settings_ menu. Go to the section, _sharing_ and turn it on. Also, activate the option _Screen Sharing_. A menu will pop up, to let you set a password and to enable VNC through wired connection.

![](/assets/images/vnc.png)

And that is all for the remote part. In the case of the local computer, the easiest option to connect is to use a software like Remmina which is usually preinstalled in the Ubuntu distribution. The process is as follows: just open Remmina, click on the add button and in the configuration panel that appears

![](/assets/images/remmina_add.png)

add the following:

+ Name: the one that you wish
+ Protocol: choose VNC
+ Server: add the IP of the remote computer. As we are, at the moment, in local network, we just have to add the internal (local) ip of the remote computer. More details ahead.
+ Username (Optional): the user name to which you connect in the remote computer
+ Password (Optional): of the user
+ Color depth: set it up to a lower value if you want a fast rate
+ Quality: same as in color

To know the local ip of the remote computer just open the command interpreter on it. Then enter `ifconfig`, the local ip is the sequence following inet, something like 192.168.*.*.

And that's it we ahve the VNC set up. The problems are that we are only working in local and that our protection is quite vulnerable.

## ssh and remote access

+ installing ssh
+ configuring ssh, ports and etc
+ looking for ips and connecting in local
+ ssh keys
+ opneing to the internet
+ public ip
+ dynamic dns in web and in the router
+ making the local computer ip static in the router
+ port forwarding
+ wake on lan in local

Note: maybe your router allows wake on lan,  mine not --> part 2: raspberry for wake s


