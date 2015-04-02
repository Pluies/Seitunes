![Seitunes screenshot](https://cloud.githubusercontent.com/assets/344835/6964138/e7404ad0-d9a2-11e4-8b4a-dde913967eac.png)

With the built-in help:

![Seitunes screenshot #3](https://cloud.githubusercontent.com/assets/344835/6964140/e74c92d6-d9a2-11e4-8b14-d85089a75328.png)

![Seitunes screenshot #2](https://cloud.githubusercontent.com/assets/344835/6964139/e748ca02-d9a2-11e4-85c0-d1a7bd7b03a9.png)

Why?
====

Because I used a MacBook with a broken screen as an ad-hoc fancy music player and accessed it through SSH. I started out using Screen Sharing to click "next" and "previous" on iTunes, but it was way too time-consuming, so here came Seitunes!

Compiling
---------

	$ make

You can also compile using clang if you have developer tools installed; just edit the makefile.

Installing
----------

	$ sudo make install

Installs the Seitunes executable inside /usr/bin (which is in your path)

Known issue - "shuffle" will be broken as the osascript files won't be moved in the correct place. Merge requests welcome :)

Starting
--------

	$ Seitunes


... That's all folks!

More documentation is available in Seitunes itself, just type 'h' to list all available commands.
