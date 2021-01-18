using renderbuffer + blit to overwrite screen content without a quad
(actually ... can we draw our UI components with this?)

https://stackoverflow.com/questions/35414826/draw-opengl-renderbuffer-to-screen
https://www.khronos.org/opengl/wiki/Framebuffer#Blitting

nvm doesn't work -- direct pixel copy, no blend
we could come up with something absurd like reading from the fb, then blending over it, then writing back

but i would imagine that would do more harm than good

https://www.gamedev.net/forums/topic/555121-fbo-resize/
https://gamedev.stackexchange.com/questions/91991/resizing-a-framebuffer-object-ie-its-attachments-on-screen-resize

these threads suggest recreating the fbo instead of resizing the underlying texture(s)

the gamedev thread even suggests using a framebuffer "repository" which stores all framebuffers so that if we need one of a particular size we can just fetch it instead of recreating it

hahaha