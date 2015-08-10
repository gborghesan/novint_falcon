# Orocos drivers for Novint Falcon #


##Prerequisite##
to compile you need the driver [libnifalcon](https://github.com/qdot/libnifalcon).
follow the instructions there to compile and __install__.


the examples in _libnifalcon_ can be run with sudo.
in order to change that (necessary for using orocos deployer), we have to add a udev rule.

copy (as super-user) the file in this repository:

udev_rule/99-udev-novint.rules -> /etc/udev/rules.d/99-udev-novint.rules

authors:

- Gianni Borghesan
- Sergio Portolez