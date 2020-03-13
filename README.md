<h1 align="center">Machinekit-HAL</h1>

<div align="center"><img alt="Machinekit demo" src="https://github.com/cerna/machinekit-hal/blob/various-bugfixes/media/machinekit-hal.svg" width="450px" /></div>

<h4 align="center">Universal <i>framework</i> for machine control.</h4>

<p align="center">
<a href="https://github.com/machinekit/machinekit-hal/actions" target="_blank">
<img alt="Github Actions build status" src="https://img.shields.io/github/workflow/status/machinekit/machinekit-hal/Test and build preview packages/master?style=for-the-badge&logo=github" />
</a>

<a href="https://travis-ci.org/machinekit/machinekit-hal/builds" target="_blank">
<img alt="Travis-CI build status" src="https://img.shields.io/travis/machinekit/machinekit-hal/master?style=for-the-badge&logo=travis" />
</a>

<a href="https://scan.coverity.com/projects/machinekit-machinekit-hal">
<img alt="Coverity Scan Build Status" src="https://img.shields.io/coverity/scan/20589.svg?style=for-the-badge" />
</a>

<img alt="Version" src="https://img.shields.io/badge/version-0.3-blue.svg?cacheSeconds=2592000&style=for-the-badge" />

<a href="https://matrix.to/#/#machinekit:matrix.org" target="_blank">
<img alt="Matrix Machinekit Room" src="https://img.shields.io/matrix/machinekit:matrix.org?style=for-the-badge&logo=matrix" />
</a>

<a href="https://groups.google.com/forum/#!forum/machinekit" target="_blank">
<img alt="Machinekit Google Groups" src="https://img.shields.io/badge/forum-Machinekit-default?style=for-the-badge&logo=google" />
</a>

<a href="http://www.machinekit.io/community/c4/" target="_blank">
<img alt="C4 community guidelines" src="https://img.shields.io/badge/contributing-C4-default?style=for-the-badge&logo=zeromq" />
</a>
</p>

<p align="center">
<a href="https://machinekit.io/">Website</a>
|
<a href="https://machinekit.io/docs">Docs</a>
|
<a href="http://www.machinekit.io/about">About</a>
</p>

**Machinekit-HAL** is a powerful software for _real-time_ control of machinery based on _**H**ardware **A**bstraction **L**ayer_ principle. With tools and libraries making development of new _components_ and _drivers_ easy. Integrators can choose to control **industrial robotic arm**, **single purpose machine** or **CNC mill** or **lathe** with additional software package.

Supporting _RT PREEMPT_ and _Xenomai 2_ real-time Linux kernel patches. **APT** packages available for Debian _Jessie_, _Stretch_ and _Buster_.

<div align="center"><img alt="Machinekit demo" src="https://raw.githubusercontent.com/cerna/machinekit-hal/various-bugfixes/media/machinekit_hal_ethercat_demo.gif" width="650px" /></div>

## Getting started

The easiest way how to get **Machinekit-HAL** running is to install Debian package. Packages can be obtained by triggering Github Actions _workflow_ and downloading **build artifacts** on your own _fork_. Or you can just download the latest builds from push to [master branch](https://github.com/machinekit/machinekit-hal/actions?query=branch:master) on official [Machinekit/Machinekit-HAL repository](https://github.com/machinekit/machinekit-hal).

Alternatively you can build locally on your machine in _**R**un-**I**n-**P**lace_ mode. The briefest sequence of commands would be:

```sh
git clone https://github.com/machinekit/machinekit-hal.git
cd machinekit-hal
debian/configure
sudo mk-build-deps -ir
cd src
./autogen.sh
./configure
make
sudo make setuid
```

More information about building can be glanced from [documentation](http://www.machinekit.io/docs/developing/machinekit-developing).

|![Warning](https://img.icons8.com/ios-filled/50/000000/warning-shield.png)| Be advised that currently there is no support for Linux distributions other than Debian. |
|:---:|---|

## History

**Machinekit-HAL** was created by separating the core functionality from now deprecated [Machinekit](https://github.com/machinekit/machinekit) repository into own repository.

It all started in the early nineties when NIST created the Enhanced Machine Controller Architecture in Public Domain as a _vendor-neutral_ software implementation for numerical control of machining operations. From that in 2003 open community of developers created a project called **EMC2** or _Enhanced Machine Controller 2_. (Or on the side of commercial software, EMC was developed into popular software _Mach3_ for Microsoft Windows.) _EMC2_ was renamed in 2011 as a **LinuxCNC**. In 2014, [**Machinekit**](https://machinekit.io) was forked from [LinuxCNC](https://linuxcnc.org) to facilitate deeper changes in _low level_ functionality. In 2020, the original _Machinekit_ repository was _archived_ and development is fully continuing in the **Machinekit-HAL** repository.

|![Warning](https://img.icons8.com/ios-filled/50/000000/warning-shield.png)| The _CNC_ part of original repository was separated into the [_Machinekit-CNC_](https://github.com/machinekit/machinekit-cnc) repository in the same move. |
|:---:|---|

## Frequently asked questions

|![Question](https://img.icons8.com/ios-filled/50/000000/ask-question.png)| What is the _**H**ardware **A**bstraction **L**ayer_? |
|:---:|---|
|![Answer](https://img.icons8.com/ios-filled/50/000000/smartphone-approve.png)| **HAL** represents one of the **fundamental** elements of _Machinekit-HAL_. One could imagine **HAL** as a _electronics breadboard_ into which _semiconductors_, _passives_ or _connectors_ (in **HAL** _componets_ and _drivers_) are inserted and connected by _wires_ (in **HAL** _signals_). This all happens **_in-memory_** and the _execution stage_ runs in Linux scheduled _thread_. |

|![Question](https://img.icons8.com/ios-filled/50/000000/ask-question.png)| Is **Machinekit-HAL** LinuxCNC? |
|:---:|---|
|![Answer](https://img.icons8.com/ios-filled/50/000000/smartphone-approve.png)| **No.** In the current state of development, we can say that both Machinekit-HAL and LinuxCNC 2.8 have a **common** ancestor. However, **Machinekit-HAL** doesn't include the CNC functionality like _LinuxCNC_, the configuration is different and the supported platforms are different also. |

|![Question](https://img.icons8.com/ios-filled/50/000000/ask-question.png)| What's the difference between **Machinekit-HAL** and Machinekit? |
|:---:|---|
|![Answer](https://img.icons8.com/ios-filled/50/000000/smartphone-approve.png)| **Machinekit-HAL** is continuation of _Machinekit_. Work on the original Machinekit repository was stopped and the only new development will happen on _Machinekit-HAL_. Machinekit-HAL exports only the core functionality to better serve the needs of _machine integrators_ and provide leaner experience for everybody. The **CNC** functionality was exported into [_Machinekit-CNC repository_](https://github.com/machinekit/machinekit-cnc). |

## Getting involved

**Machinekit-HAL** like all projects in the **Machinekit** organization is volunteer based governed by the [**C**ollective **C**ode **C**onstruction **C**ontract ](http://www.machinekit.io/community/c4), generally known as a C4 originally from the [ZeroMQ](https://rfc.zeromq.org/spec/22) project.

The source code is hosted publicly on [GitHUB](https://github.com/machinekit/machinekit-hal), where majority of programming discussion about further development happens. In lower measures, Machinekit-HAL is also discussed on [_Machinekit forum_](https://groups.google.com/forum/#!forum/machinekit) and in [_Machinekit Matrix Room_](https://matrix.to/#/#machinekit:matrix.org), which are used more to the point of support platforms and for general chat.

|![Counselling](https://img.icons8.com/ios-filled/100/000000/counselor.png)|
It's always encouraged to create a new _issue_ in GitHub tracker first. Discuss the proposed changes there and then based on the output implement the changes and create a new _pull request_. |
|:---:|---|

## Licence

This software is released under the **GPLv2**, with some parts under the **LGPL**. See the file COPYING for more details.

|![Warning](https://img.icons8.com/ios-filled/50/000000/warning-shield.png)| For more detailed information consult specific files with source code implementing given functionality. There should be explicit licensing. |
|:---:|---|