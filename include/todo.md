# roadmap

- template out a hierarchy
  - windows (standard window shit)
    - figuring out how to do a borderless window would be cool as shit (oneshot solstice ending)
    - supporting multiple windows within a single process would be neat!
    - wrap gl calls to create windows programmatically
    - methods
      - render (render all layers appropriately)
      - addLayer
      - thats about it really
  - screens
    - components which can be rendered individually
    - maybe make some "toFront" and "toBack" methods to specify which screens should appear in front of which other screens
    - individual frame buffers
      - render method: after receiving an X and Y component, render yourself.
    - probably a class
    - groups!
      - a group renders its components to a framebuffer as if it were its own screen
      - only if necessary :)
    - methods
      - setX/Y/width/height (XY from top left probably)
      - render (based on current state)
      - getBuffer
      - group:
        - addLayer
      - keep it like this for now and add calls as needed

  