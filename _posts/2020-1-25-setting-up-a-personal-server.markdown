# Setting up a personal GPU server. Part 1

In this post I want to explain a little bit my vicissitudes in setting up my desktop computer as a _server_ for GPU computation. Of course, I assume you already have your GPUs working on your computer.  The idea is also to show the steps that I followed in a progressive manner: from VNC in the local network to a ssh accesible computer across the net. Unfortunately for windows users, this guide/story is only for Ubuntu based computers.

## Local VNC

[VNC](https://en.wikipedia.org/wiki/Virtual_Network_Computing) stands for Virtual Network Computing and allows for taking remote control of computer. Basically, it allows to share the graphical desktop. One strength is that it is platform independent. The main weakneses are its graphical nature and its limitation for passwords. The first does not make it useful in poor connection environments and the second, with its limitation of 8 characters for passwords, make it somehow insecure to use in an open network or across the internet.

The set up is easy. First open the _settings_ menu. Go to the section, _sharing_ and turn it on. Also, activate the option _Screen Sharing_. A menu will pop up, to let you set a password and to enable VNC through wired connection.

![]({{site.baseurl}}/img/ssh-server-1/vnc.png)

And that is all for the remote part. In the case of the local computer, the easiest option to connect is to use a software like Remmina which is usually preinstalled in the Ubuntu distribution. The process is as follows: just open Remmina, click on the add button and in the configuration panel that appears

![]({{site.baseurl}}/img/ssh-server-1/remmina_add.png)

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

## SSH and remote access

A better option to access a remote computer shell either in a local environment or through the net is SSH. SSH stands for Secure Shell and it is used to communicate with and administrate servers. Let's install ssh server.

```
apt-get update
apt-get install openssh-server
``` 
SSH starts automatically after installation. We can check that it is working with

```
sudo systemctl status ssh
```

It should appear something saying that it is active.  Like in the following image


![]({{site.baseurl}}/img/ssh-server-1/status.png)

We can also restart the service as `sudo systemctl restart ssh`. Same for commands such as `enable`, `disable`, `stop` and others.

**NOTE** If you have the firewall in ubuntu active you should allow ssh to go through it with `sudo ufw allow ssh`.

### 1. Local Connection

Now, in the VNC section we checked our localip with `ifconfig. In the last piscture we could see also the port at which the ssh service was working (by default ssh is palced at port 22, but it can be changed. Indeed, it is a good practice to do so). With these two datum we can connect to our computer from another one in the same LAN (Local Area Network) with the following command:

```
ssh -p <port_number> <user_name>@<ip>
```

The first time it will ask if we want to connect to that machine and, of course, we will say yes. If everything goes fine we should be able to connect without problems.

### 2. Configuring the ssh service

In `etc/ssh/sshd_config` is the configuration of the ssh server. There are lots of options but I have only tackled some of them:

+ Port: port at which the service will be connected. Be aware when you choose a number that some ports are already in use. The recomendation is to use the standard
one, 22, or a number beyond 50000. In [here][reference] you can see that some ports are for dynamic usage.
+ X11Forwarding: if you want to allow to forward the GUI.
+ PasswordAuthentication: if you want to alow accesing the computer with a password. We will leave it by now as a yes. Afterwards, we will allow only connections by ssh-keys use.

There are other configurations such as `AuthorizedKeysFile` which we will use later. At the moment it will be sufficient with those three options

### 3. Opening to the internet

#### Public IP
#### Port forwarding
#### Making the ip static
#### Dynamic DNS
#### ssh key



+ installing ssh
+ configuring ssh, ports and etc
+ looking for ips and connecting in local
+ opneing to the internet
+ public ip
+ dynamic dns in web and in the router
+ making the local computer ip static in the router
+ port forwarding
+ ssh keys

## References

+ [reference](https://hostadvice.com/how-to/how-to-change-your-ssh-port-from-the-default-for-security/)
Note: maybe your router allows wake on lan,  mine not --> part 2: raspberry for wake s


