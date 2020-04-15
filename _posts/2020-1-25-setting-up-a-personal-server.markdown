---
layout: post
title: 'Setting up a personal GPU server. Part 1'
date: '2020-4-16'
categories: jekyll update
published: true
subtitle: >-
    Would you like to be able to connect to your GPU station from anywhere, anytime?
    Run programs remotely and have a centralized server? Here I Show my experience building up a personal server.
---

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

If you had any problems there are [tons][2] of webs describing how to set up your ssh service.

### 2. Configuring the ssh service

In `etc/ssh/sshd_config` is the configuration of the ssh server. There are lots of options but I have only tackled some of them:

+ **Port**: port at which the service will be connected. Be aware when you choose a number that some ports are already in use. The recomendation is to use the standard
one, 22, or a number beyond 50000. In [here][1] you can see that some ports are for dynamic usage.
+ **X11Forwarding**: if you want to allow to forward the GUI.
+ **PasswordAuthentication**: if you want to alow accesing the computer with a password. We will leave it by now as a yes. Afterwards, we will allow only connections by ssh-keys use.

There are other configurations such as `AuthorizedKeysFile` which we will use later. At the moment it will be sufficient with those three options

### 3. Opening to the internet

Now we have a service which is only working in local. To open to the whole internet (with the risks that it conveys) we must follow certain steps. 

#### Public IP

If we go to a browser and write `ip` it will appear our public ip. Or you can just check it [here][3]. Basically this is our public ip, that is, the direction of the entry point to our LAN. Or what is the same the direction by which our router is identified. 

Hence, two questions arise: 

+ How do we connect our computer to the ip of the router?
+ IS this direction fixed?

For the first question we will need something called port forwarding. This means that when we call our router with a ssh petition at certain ports he will know to which ports (i.e. our computer) send the petition. In the second case, the answer is not and we will have to activate something called dynamic DNS to ensure that we can connect any time.

#### Port forwarding

First, let's forward ports from the router to the ports of our computer where the service is active. First, we have to access our router configuration page and for this we need the ip of our router. With a [one-liner][4]

```
netstat -nr | awk '$1 == "0.0.0.0"{print$2}'
```

we can obtain it. If we introduce it in the web browser we can reach the router configuration page. There we have to do the following: search for the port forwarding configuration. In my case it was under the NAT section. There, we have to establish several things:

+ **The service**: in our case the SSH
+ **The external ports**: you can choose some ports outside the main usage range
+ **The internal ports**: the ports that you defined in the sshd configuration
+ **The internal ip**: the ip towards which the request must be forwarded
+ **The protocols**: I have defined both TCP and UDP, but can work just with TCP

In the ports it will ask for a range but you can put the same value for the start and for the end. And that is it, we have told our router that, when a ssh request comes to it it must forward it to the specific ip and port.

[Here][6] you can find a nice explanation of port forwarding and how to configure it in a sample router. However, each router is a world and you will ahve to find your own way.

#### Making the ip static

In our router we have to do something else for this setup to work. The local ip of the computer that we have established with the `ssh-server` is not static by dynamic. That is, it is changed regularly by the router with a protocol called [DHCP (Dynamic Host Configuration Protocol)][5], which is in charge of managing the distribution of ips over a network.

As we want our computer to be reached, we need to establish that ip as fixed. Hence, in our router configuration we must look for something such as `Static IP List` or similar. In my case it was under the section LAN and under the name Static IP Lease List. There we introduce the current IP of the computer where we ahve the ssh-server, as it will not enter in the next reorganization of ips.

In the next image we can see the configuration that I have: in the `IP Address` I have the local ip of the computer with the ssh server (recall that we know how to find it from the VNC section)

![]({{site.baseurl}}/img/ssh-server-1/static_local_ip.png)

[Here][7] you can find extra counsels and recommendations for making your ip static.

#### Dynamic DNS

Until now we have:

+ Set up the ssh server
+ Configured it
+ Established the port forwarding directives
+ Made the local ip of the computer with ssh server static

By now, we can ssh our computer from an outside point of our network (for example, give internet to your laptop with your mobile phone and try to reach the computer), just by:

```
ssh -p <external port of router> <username>@<external ip>
```

Perfect. Is there any problem? Yes, of course. In the same way that your router manages the local ips with a DHCP server in your local network, your external ip is changed dynamically by your ISP (Internet Service Provider). Hence we have to find a way to fix it and avoid the dynamism or go around that. 

The solution does not fix the ip but really goes around. 

First, DNS (Domain Name System) is a mapping between IPs and Domain Names. Fro example, the IP of the Domain Name Facebook.com is X. Hence, instead of typing the ip of the service or web we are trying to reach we just write the domain name and a DNS Server resolves the IP. For a company it is easy to set the domain name and forget about the ip since the IP is static. However, for a residencial service, your ISP has a pool of IPs and it changes your IP dinamically. Thus, your external IP is changing regularly. What we can do is to set up a Domain Name which will follow the changes in the IP from now on. This is called Dynamic DNS and is easy and free (up to a point) to [set up][8] .

There are a lot of services [out there][9]. I have chosen NoIP but you could choose the one you prefer. There we have to put our current IP and define a Domain Name (or Hostname). But that is not all, we also have to tell our router that we have associated its external IP with a Dynamic DNS service. For this we have to search in the router's configuration page,in the same way as before, for a section devoted to this topic as in the following image:

![]({{site.baseurl}}/img/dyndns.png)

where it requires the name of the DYn DNS service, the hostname and of course the username and passwords of the DYn DNs service.

With this last step we have a fully functioning ssh server, accesible from everywhere in the internet. Wait... Everywhere? Yes, that is right we have our server listening for ssh requests on a certain port. Maybe we should make it as secure as possible. Yes, and that is the reason for the next and final step.

#### ssh key

Now the server is accesible as: `ssh -p <port> <username>@<hostname>`. It demands our user password and that is it: we are in. However, this is not the most secure way to access our server. A better and more secure way is with [SSH keys][10].

SSH keys work in pairs: public and private. The public key can be shared and made public but the private should rest well preserved. It can (should) be locked down with another password indeed. 

To use SSK keys, what we do is the following: we create a public-private pair in the computer we will use to connect to the remote host. Then we send the public key to the remote host, which will save it and use it to decrypt our SSH petitions. Tpublic key is usually saved in `~/.ssh/authorized_keys`. Now, every time that we connect to our host with computer we won't need the password because the check will be made through the ssh keys. 

To activate SSH keys and deactivate the password access, we must go to the ssh server configuration file `/etc/ssh/sshd_config` and set 

+ `PasswordAuthentication no`
+ `PubkeyAuthentication yes`

To create an SSH pair in Ubuntu is easy (there are good alternatives in Wndows such as Puttygen to use with Putty). 

1. Create the key pair in your local computer entering the following in the command line

    `ssh-keygen`

2. You can protect your private key with a password and the process will also warn you, if that is the case, that your are overwriting your current key. Be careful with that.
3. Your public key will be now at `~/.ssh/id_rsa.pub`. We have to sent this to our server.
4. We can use the authoatic way if we have still password access `ssh-copy-id -p <port> <username>@<remote_host>` 

    We can also copy it manually by doing the following

    `cat ~/.ssh/id_rsa.pub | ssh username@remote_host "mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys"`

    which basically pipelines the output of cat command (in this case the display of our key) to the ssh call which creates, if it does not exist, the `.ssh` directory and appends the previous output to the `~/.ssh/authorized_keys` file.

5. If we already have shut down password access we will have to do the previous step manually. That is, accessing the server and copying the key to the `~/.ssh/authorized_keys` file.


And that is it, we have set the ssh-keys and we have a more secure ssh server. However, there a more ways to secure it further such as `fail2ban`, with this we are good to go.

## Conclusion

In this project we have been able to set a ssh server in our desktop computer. With this we will be able to run our programs from anywhere directly in our computer. We just connect to our server:

`ssh -p <port>  <username>@<hostname>`

and we are inside running our programs.

In the process we have learnt about:

+ VNC
+ SSH and its configuration
+ Network fundamentals: 
    + external and internal IPs
    + DHCP
    + port forwarding
    + static IPs 
    + router configuration
    + DNS and dynamic DNS
+ SSH keys securing

However, there is a down side for this system. The server is always up and running, which can be good for a server which is used 24/7/365 but for a home service we may want to use it when it is of convenience. For this in part 2 we will make use of WOL (Wake On Lan) and a Raspberry Pi to act as an always awoke server, while our heavy server is sleep. LEt's get to it.

[1]: https://hostadvice.com/how-to/how-to-change-your-ssh-port-from-the-default-for-security/ "Recommended port numbers"
[2]: https://linuxize.com/post/how-to-enable-ssh-on-ubuntu-18-04/ "Set up ssh"
[3]: https://whatismyipaddress.com/ "my public ip"
[4]: https://askubuntu.com/questions/605424/how-to-show-just-the-ip-address-of-my-router "ip of router"
[5]: https://www.lifewire.com/what-is-dhcp-2625848 "dhcp"
[6]: https://www.howtogeek.com/66214/how-to-forward-ports-on-your-router/ "Port forwarding"
[7]: https://www.howtogeek.com/184310/ask-htg-should-i-be-setting-static-ip-addresses-on-my-router/ "static local ip"
[8]: https://www.howtogeek.com/66438/how-to-easily-access-your-home-network-from-anywhere-with-ddns/
[9]: https://www.maketecheasier.com/best-dynamic-dns-providers/
[10]: https://www.digitalocean.com/community/tutorials/how-to-configure-ssh-key-based-authentication-on-a-linux-server

Note: maybe your router allows wake on lan,  mine not --> part 2: raspberry for wake s


