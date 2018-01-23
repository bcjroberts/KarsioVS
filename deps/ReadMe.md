I (Sebastian) have restructured the includes and 
libs so that it's easier to add additional libraries without
having to manually add them to the project every time.

Basically now instead of including every library individually
all the includes (header files) from libraries are in one 
place and all the libs from those libraries are in one place.
Thus they are all included in visual studio in a single filepath  

I've excluded physx since while the PxShared stuff could work
with minimal effort, physx's includes are not setup to be in
a subfolder. Which would make the include folder really messy.


Technically ..OldDepsFolders can be deleted but I don't want
to be the one to make that decision :D 