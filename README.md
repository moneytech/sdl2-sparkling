# SDL2-Sparkling

## Guess what, SDL2 bindings for Sparkling

### Basic usage

1. Import SDL
2. Open window(s) to draw on
3. Draw window; render
4. Handle events
5. goto 3

<!-- commity-comment -->

    // 1.
    let SDL = dynld("sdl2");

    // 2.
    let window = SDL::OpenWindow("Title", optionalWidth, optionalHeight);

    while true {
        // 3.
        window.fillRect(0, 0, window.width, window.height);
        window.refresh();

        // 4.
        var event;
        while (event = SDL::PollEvent()) != nil {
            // do something with 'event' here...
        }

        // 5.
    }

### Functions

    OpenWindow(title [, width, height])

Opens a new window that you can draw on, returns the representing
window object. If width and height are omitted, window is opened
in fullscreen mode; `width` and `height` properties of window object
will reflect resolution of screen. The `ID` property of the window
object is an integer ID used throughout the event system.

    PollEvent()

Returns an event object if there are any pending events to handle;
otherwise, returns nil. The object has the following properties:

 - `type`: string describing the type of the event
 - `timestamp`: implementation-specific integral timestamp

Depending on the `type` of the event, the following additional
properties are also available:

 - if type is `timer`, then `ID` will contain the timer object that
   generated the event.
 - if type is `quit`, then there are no additional properties.
 - if type is `window`, then:
   - `ID` is the integer window ID of the window that generated the event
   - `name` is the event name as a string, e. g. `focus_gained`,
     `focus_lost`, `resized`, etc.
   - `data1` and `data2` are two integers. Their semantics is dependent
      on the event name; see
      [the documentation of SDL_WindowEventID](https://wiki.libsdl.org/SDL_WindowEventID)
      for more info.
 - if type is `keyboard`, then:
   - `value` is a string describing the key that sent the event
   - `state` is `true` if the key was pressed, `false` if it was released
   - `modifier` is a hashmap of booleans with keys like `lshift`,
     `rshift`, `lctrl`, `lctrl`, etc. (see source code for all keys).
	 These flags represent the status of the modifier keys pressed
	 simultaneously with they key.
 - if type is `mousebutton`, then:
   - `button` is the name of the button
   - `state` is `true` for a pressed button, `false` for a released button
   - `count` is the (integer) number of clicks
   - `x` and `y` are the coordinates of the click
 - if type is `mousemove`, then:
   - `x` and `y` are the coordinates of the cursor
   - `dx` and `dy` are the relative movements since the last motion event
   - `buttons` are a set of Boolean flags, describing the state of the
     mouse buttons (see source for more info).
 - if type is `mousewheel`, then `x` and `y` are the relative changes in
   the motion of the wheel.
 - if type is `touchdown`, `touchup` or `touchmove`, then:
   - `finger` is an implementation-defined unique ID of the finger sending
     the event
   - `x`, `y`, `dx` and `dy` are normalized (to the [0...1] closed interval)
     values of the coordinates and relative displacement of the finger,
	 respectively
   - `pressure` is the applied force (normalized to the interval [0...1])
 - if type is `gesture`, then:
   - `fingerCount` is the number of fingers participating in the multi-touch
     gesture
   - `x` and `y` are the normalized coordinates of the centerpoint/centroid
     of the touches that form the gesture [0...1]
   - `rotation` is the angle of rotation, in radians, since the last gesture
     event
   - `distance` is the normalized [0...1] movement of the fingers since
     the last gesture

<!-- commity-comment -->

    StartTimer(interval [, callback])

Sets up a timer on a background thread and returns the corresponding timer
object. `interval` is to be specified in seconds (may be fractional).
If present, `callback` must be a function which will be invoked on the
background thread everytime the timer fires. If omitted, the timer
will generate a `timer` event instead (see above).

If the returned timer object is deallocated, the corresponding timer
object is stopped. (so, if you want to keep the timer alive, you must
hold a reference to the timer object, for example by storing it in a
variable or a data structure.)

    StopTimer(timer)

Stops the timer associated with `timer`. `timer` must be a timer
descriptor object returned by `StartTimer()`.

### Drawing primitives

The `Window` class (the type of objects returned by `OpenWindow()`)
has the following methods:

    refresh()

Actually renders the backing memory buffer to the screen and updates
the window. Don't call this in a tight loop! (only once per frame)

    setColor(r, g, b, a)

Sets the current drawing color of the window in RGBA format. All
parameters are floating-point numbers between 0 and 1.

    getColor()

Returns a hashmap with keys `r`, `g`, `b`, `a`. The values are
normalized ([0...1]) floating-point numbers.

    setBlendMode(mode)

Sets the current drawing blend mode in order to specify how the alpha
channel is used. `mode` is one of SDL's four Blend Modes:
"none", "blend", "add" or "mod"

    getBlendMode()

Returns the name of the blend mode being currently used.

    setFont(name, ptsize, style)

Sets the active font for text rendering. `name` is the name of the font,
it will be used for constructing the font file name to be loaded (by
appending the string `.ttf` to it). `ptsize` is the font size in points,
where 72 points = 1 inch. `style` is a whitespace-separated list that
consists of any of the following substrings:

 - `normal`
 - `bold`
 - `italic`
 - `underline`
 - `strikethrough`

<!-- commity-comment -->

    clear()

Fills the entire window with the current drawing color.

    strokeRect(x, y, w, h)
    fillRect(x, y, w, h)

Draw a rectangle outline or a filled rectangle, respectively,
at coordinates (x, y) of size (w, h).

    strokeArc(x, y, r, start, stop)
    fillArct(x, y, r, start, stop)

Stroke or fill an arc with center point (x, y), radius r, starting
angle start and end angle stop. Angles are measured in radians.

    strokeEllipse(x, y, rx, ry)
    fillEllipse(x, y, rx, ry)

Draw an ellipse with center point (x, y), horizontal semi-axis rx
and veritcal semi-axis ry.

    fillPolygon(x1, coords)

Fills the polyon enclosed by the points at coordinates `(x1, y1)`,
`(x2, y2)`, `(x3, y3)`, etc., where `x1, y1, x2, y2`, etc. are
consecutive numbers in the `coords` array.
At least 3 points (6 coordinates) must be specified.

    strokeRoundedRect(x, y, w, h, r)
    fillRoundedRect(x, y, w, h, r)

same as `strokeRect()` and `fillRect()`, except the corners of the
rectangles will be rounded, with radius `r`.

    bezier(steps, coords)

Draws a Bezier curve through the specified points, using `steps` steps
to interpolate between two consecutive points. `steps` must be at least
2, the number of points must be at least 3. The coordinates are specified
using the `coords` array. Every element of the array must be a number;
pairs of consecutive numbers are the X and Y coordinates of the next
point of the curve, i. e. `[x1, y1, x2, y2, x3, y3, ...]`

    line(x, y, dx, dy)

Draws a line starting at point (x, y), moving along the vector (dx, dy).

    point(x, y)

Draw a single pixel at point (x, y)

    renderText(text, hq)

Renders the string `text` using the current drawing color and current
font. if `hq` is `true`, the rendering will be higher-quality but slower
than if it was `false`. Returns the texture with the rendered text.
(this function does not actually do drawing -- in order to blit the
resulting texture to the window, use `renderTexture()`.)
This function raises a runtime error if currently there's no font set
in the window.

    textSize(text)

Returns a hashmap with keys `width` and `height` which are integers
specifying the size of the given `text` rendered using the current
font. (No actual rendering is done, only the computation of the
font size is carried out using kerning.) Raises a runtime error if
there's no font set in the window currently.

    renderTexture(texture, x, y)

Blits the contents of `texture` at point `(x, y)` to the window.

    loadImage(filename)

Loads the file at `filename` into memory. Returns the
resulting texture object on success and `nil` on error.

    linearGradient(w, h, dx, dy, colorStops)

Draws a linear gradient inside a rectangle of size `w * h`.
and returns the resulting texture. `dx` and `dy` are components of the
direction vector of the line along which you wish to draw the
gradient. `colorStops` is an array of hashmaps. Each element of
the array must have at least the following keys-value pairs:

 - `r`, `g`, `b`, `a`: normalized (1…0) (usually floating-point)
   components of the color at the corresponding color-stop point.
 - `p` is the normalized progress/ratio of the color stop object,
   also a number between 0 and 1.

There must be at least 2 color-stop points. Here's how such a
linear gradient looks like:

![linear gradient](linear_gradient.png)

<!-- commity comment -->

    radialGradient(rx, ry, colorStops)
    conicalGradient(rx, ry, colorStops)

These functions draw a radial or conical gradient, respectively.
`cx` and `cy` are the coordinates of the enclosing ellipse.
`rx` and `ry` are the horizontal and vertical semi-axes thereof.
`colorStops` have the same purpose and layout as discussed above.
The returned texture object can be rendered using `renderTexture()`.

Here's an example of a radial gradient:

![radial gradient](radial_gradient.png)

And a conical one:

![conical gradient](conical_gradient.png)

## Examples

For example code, see the Sparkling files:

- [`example_docsig.spn`](example_docsig.spn)
- [`example_graph.spn`](example_graph.spn)
- [`demo_linear_gradient.spn`](demo_linear_gradient.spn)
- [`demo_radial_gradient.spn`](demo_radial_gradient.spn)
- [`demo_conical_gradient.spn`](demo_conical_gradient.spn)
