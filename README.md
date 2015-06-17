Command Line Interface
======================

This code goes way back so do not crucify me over it. And currently is completely 
in C++98 land. 

Here's the current todo list:

- Get rid of boost and update the code to C++11
- Support UTF-8 terminal locales (linux) and figure out internal library Unicode support  (currently only ASCII)
- Better keyboard support for raw/cooked modes. 
- Default keyboard mapper?
- Improved terminal features?


What It Is?
===========

![Screenshot](https://raw.githubusercontent.com/ensisoft/cli/master/screens/linux_terminal_app.png "Linux terminal"))


cli is a small library for writing simple console user interfaces
using "high level" widget like interfaces. It has been designed to be simple to
use, extend, understand and integrate. It has also been designed to be 
completely abstract and therefore doesn't do any input or output by itself 
directly. Instead all input is passed into the library using virtual key semantics
and all output from the library is passed back to the application inside an 
abstract "frame buffer" object.
This means that host application will be responsible for using whatever means 
it wants for mapping user input to the virtual keys understood by the library. 
Also in order to display any data the host application will be responsible 
for transferring (printing) the output form the library
to the actual terminal screen buffer for display using whatever means it wants.
While this may sound like a burden and something that makes the library cumbersome to use
the reality is actually qclite the opposite. This design makes the library
very portable, testable and flexible. Say you want double buffering for instance, 
easy to do. Or you want to script your widgets? Well thats easy to do too!
Ideally there exists another library that provides a uniform interface to a bunch
of terminals (and terminal like environments) for a multitude of platforms.
Unix, Linux, Windows etc. With this library one could access the terminal contents,
manipulate the cursor and use fancy techniques such as colors and other video
properties. However this library is not that library, since this is just a widget library.

Someone might also ask why the library doesn't provide a "main" loop. In my opinion
most applications have such a wide variety of needs about what they want to do in their 
main loop, that it simply is the best and most correct solution to leave it up the client.
Any non-trivial application might want to for example, wait for user input,
wait on any other objects such as sockets, pipes, file handles or do polling or any
other kind activity in the application loop. Massaging all this information and context
into any "generic" library defined main loop becomes tricky really qclickly. Not to mention
a case where one wants to use combine several libraries each having their own perverted
notion of what a "main loop" should look like. 

In my opinion in any application/library design the library should just provide the tools 
for doing stuff and the application should provide the context; the glue for putting things together
and managing the activities between different components. 
Anyway, enough with the jabbering and on with the real stuff...

The library depends only on the standard C++ environment and the awesome boost 
C++ package (no one should code c++ without boost!).
It was developed against boost 1.33.1 but porting to newer versions should
be straight forward.

The library was originally developed as a subproject of a bigger project.
Thats why it has crappy naming and stuff. This should be easy to fix though 
should there ever be need for that.

For more information see the sample code.

The library is distributed under the MIT license.

The current build uses Boost.build and Jamfiles are provided. 
However migrating the code over to any other build system should be trivial.

The library is not all perfect and not fully optimized. There's plenty of
needless drawing going on. However so far this hasnt been a major issue,
but at some point if you're working with a terminal with slow IO system
this might become essential. Also im not sure if the current way of doing 
"drawing" is good, or whether it should be changed for message based/queued notifications. 
Regardless if you have comments/suggestions/fixes please dont hesitate to 
send them to me if you want.

Current features:
-----------------

ASCII support only
Animation support
- scrolling text support
Policy based design
Input policies
- alphanumeric input
- numeric input
- password input
Tab completion policies
- filebrowser tab completion policy (should actually live outside the lib's scope)
- basic tablist
Widget/window events
- focus
- draw
- cursor
- keydown
- select
- click
- easy to add more events as needed
Color support
Video properties support
- reverse video
- underline
List/table selection modes
- single select
- multi select
Menus

