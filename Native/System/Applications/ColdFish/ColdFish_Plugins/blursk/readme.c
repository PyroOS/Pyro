/* Automatically generated from README, via Makefile.am */
char readme[] =
	"                                    Blursk 1.3\n"
	"\n"
	"          Copyright (c) 2002 by Steve Kirkendall\n"
	" Freely redistributable under the Gnu GPL version 2 or later\n"
	"\n"
	"                   <kirkenda@cs.pdx.edu>\n"
	"          http://www.cs.pdx.edu/~kirkenda/blursk/\n"
	"\n"
	"\n"
	"Blursk is a visualization plugin for XMMS.  It was inspired\n"
	"by the \"Blur Scope\" plugin, but Blursk goes far beyond that.\n"
	"It supports a variety of colormaps, blur patterns, plotting\n"
	"styles, and other options.  The only things that haven't\n"
	"changed are parts of the XMMS interface and configuration\n"
	"code.\n"
	"\n"
	"To configure Blursk, select it in the Visualization Plugin\n"
	"menu, and then click the [Configure] button, as usual.  If\n"
	"Blursk is running while you're configuring it, then you can\n"
	"see the effects of your changes immediately as you make them.\n"
	"\n"
	"Right-clicking on the Blursk window will also bring up the\n"
	"configuration dialog.  Dragging the left mouse button will\n"
	"move the window, and Blursk will even remember that position\n"
	"the next time you start it.\n"
	"\n"
	"Blursk allows you to store combinations of settings as a\n"
	"\"preset\".  The preset controls are located at the top of\n"
	"the configuration dialog, above the options.  The controls\n"
	"consist of a combo box where you can type in the name of a\n"
	"preset, and three buttons for loading/saving/erasing the\n"
	"preset named in the combo box.  Changing the name in the\n"
	"combo box has no direct affect on the configuration; you\n"
	"must press one of the buttons to make something happen.\n"
	"\n"
	"As a special case, loading \"Random preset on quiet\" will\n"
	"cause Blursk to randomly choose one of the other presets\n"
	"each time silence is detected on the input.  You can end the\n"
	"random choices by loading any other preset, or by altering\n"
	"any property of the current preset.\n"
	"\n"
	"Blursk also supports a variety of full-screen modes.  First\n"
	"you must configure the full-screen method by clicking the\n"
	"[Advanced] button on the Configure dialog.  (The full-\n"
	"screen options are described at the end of this file.)\n"
	"After that, you should be able to switch to full-screen\n"
	"mode by typing <Alt-Enter> or <F> in the Blursk window.\n"
	"To switch back, hit <Alt-Enter> again.\n"
	"\n"
	"=============================================================\n"
	"COMMANDS\n"
	"-------------------------------------------------------------\n"
	"\n"
	"Blursk's animation window is sensitive to the following\n"
	"keys.  These should work in full-screen modes too, except\n"
	"when Blursk is running in the root window.\n"
	"\n"
	"    <F> or <Alt-Enter>  Toggle full-screen mode on/off\n"
	"           <I>          Show track info\n"
	"        <Z> or <Y>      Previous track\n"
	"           <X>          Play\n"
	"           <C>          Pause\n"
	"           <V>          Stop\n"
	"           <B>          Next track\n"
	"           <Up>         Increase volume\n"
	"          <Down>        Decrease volume\n"
	"\n"
	"The mouse is also supported.  The following actions work\n"
	"in the Blursk window or full-screen mode.  Most of these\n"
	"work when Blursk is running in the root window too, if\n"
	"the mouse is in the [Blursk] button.\n"
	"\n"
	"* Drag an edge of the window to resize it.\n"
	"* Drag the interior of the window to move it.\n"
	"* While in full-screen mode, click on the window to revert\n"
	"  to window mode.\n"
	"* Right-click to bring up the configuration dialog.\n"
	"* Middle-click to paste a configuration string into Blursk.\n"
	"* Move the scroll wheel to change the volume.\n"
	"\n"
	"=============================================================\n"
	"CUT & PASTE\n"
	"-------------------------------------------------------------\n"
	"\n"
	"Blursk supports \"cut & paste\" as a convenient way to share\n"
	"settings.  It uses a short string to represent the current\n"
	"settings; this string becomes the \"Primary Selection\" for\n"
	"your workstation.  You can paste this string into email,\n"
	"web pages, or wherever.  If you paste a string into Blursk,\n"
	"Blursk will load the settings encoded in the string.\n"
	"\n"
	"COPY:  To copy the current settings into a document, first\n"
	"       bring up the Configure dialog by right-clicking on\n"
	"       the Blursk window.  Then simply push in the [Copy]\n"
	"       button at the bottom of the dialog.\n"
	"\n"
	"       The [Copy] button will remain pushed in until you\n"
	"       click the [Copy] button again, or until some other\n"
	"       application claims the primary selection.  As long\n"
	"       as the button remains pushed in, you should be able\n"
	"       to paste into any other application, such as a text\n"
	"       editor.  (Many applications use the middle mouse\n"
	"       button to paste.)\n"
	"\n"
	"PASTE: To load settings from another document, select the\n"
	"       string in the usual manner.  (For most applications,\n"
	"       this simply means dragging the mouse through the\n"
	"       text -- the highlighted text immediately becomes\n"
	"       the primary selection.)\n"
	"\n"
	"       Then middle-click on the Blursk animation window.\n"
	"       (Not the Configure dialog!)  Blursk will parse the\n"
	"       string, and adjust its configuration to match\n"
	"       whatever it was able to parse from the string.\n"
	"\n"
	"=============================================================\n"
	"OPTIONS\n"
	"-------------------------------------------------------------\n"
	"\n"
	"BASE COLOR\n"
	"    This allows you to select the drawing color.  Some\n"
	"    colormaps add extra colors, but all of them will use\n"
	"    this color as a reference in one way or another.\n"
	"\n"
	"COLOR OPTIONS\n"
	"    The first item here is the color style.  It controls the\n"
	"    way that Blursk will generate a color map.  In addition\n"
	"    to a variety of hardcoded color styles, there is also a\n"
	"    \"Random\" setting which causes one of the other color\n"
	"    maps to be chosen at random; in addition, each time the\n"
	"    blur motion changes (see below), a new color map will be\n"
	"    chosen.\n"
	"\n"
	"    The fade options come next.  Images are always drawn in\n"
	"    the color at one end of the color map (usually the\n"
	"    brightest end); over time, the image color is shifted\n"
	"    toward the other end of the color map.  This option\n"
	"    controls the speed at which this shifting takes place.\n"
	"\n"
	"    The next option determines which color the signal will\n"
	"    be drawn in.  \"Normal signal\" uses the brightest color,\n"
	"    \"White signal\" forces the brightest color to be white,\n"
	"    and \"Cycling signal\" causes the signal to be drawn in\n"
	"    different colors.\n"
	"\n"
	"    Setting the \"Contour lines\" flag will add some other\n"
	"    white lines to the color map.\n"
	"\n"
	"    Setting \"Hue on beats\" will cause a different base color\n"
	"    to be chosen when Blursk detects beats in the music.\n"
	"    Unfortunately, the beat detector isn't very good yet.\n"
	"\n"
	"    The default background color is \"Black backgnd\", but\n"
	"    you can also choose \"White\" (really light gray),\n"
	"    \"Dark\" (a dark version of the base color), \"Shift\"\n"
	"    (120 degrees around the color wheel from the base\n"
	"    color), \"Color\" (a random color), or \"Flash\" (the\n"
	"    background color flickers in response to the music).\n"
	"\n"
	"BLUR OPTIONS\n"
	"    Blur motion is the first option in this group.  It\n"
	"    determines the way that the image's pixels drift.\n"
	"    There are many blur motions supported here, plus\n"
	"    \"Random\", \"Random slow\", and \"Random quiet\" settings\n"
	"    which cause one of the other blur motions to be chosen\n"
	"    randomly, either at regular intervals or at the start\n"
	"    of a quiet period.\n"
	"\n"
	"    Next is the switching speed.  When you change blur\n"
	"    styles (or \"Random\" does it for you), the new style\n"
	"    doesn't instantly replace the old style.  It happens\n"
	"    gradually, under control of the switch speed option.\n"
	"\n"
	"    The third option in this section controls the type of\n"
	"    blurring.\n"
	"\n"
	"    Next is the \"stencil\" option.  Blursk contains a\n"
	"    variety of bitmapped images; this options allows you\n"
	"    to incorporate one of the images into the blur motion\n"
	"    in a subtle way.  \"Random stencil\" chooses one of the\n"
	"    images randomly whenever the blur motion changes.\n"
	"    \"Maybe stencil\" does the same except that it usually\n"
	"    chooses no image at all.\n"
	"\n"
	"    The \"Slow motion\" option cuts the frame rate in half.\n"
	"    This slows down the image's motion, and it also reduces\n"
	"    the CPU load.\n"
	"\n"
	"EFFECTS\n"
	"    The first option in this section controls the way that\n"
	"    the sound signal is converted to an (X,Y) position in\n"
	"    the window.\n"
	"\n"
	"    The next option controls the way that those points are\n"
	"    plotted on the window.  The \"Radar\" setting is unusual\n"
	"    in that it uses the X value as a radius, and the Y value\n"
	"    as a brightness.  \"Edges\" also uses Y as a brightness,\n"
	"    and uses X to select a position along the perimeter of\n"
	"    the window.\n"
	"\n"
	"    The \"Thick on beats\" flag attempts to make the renderer\n"
	"    use thicker lines when the sound is loud.  For some\n"
	"    combinations of options, this can make the entire image\n"
	"    seem to throb in step with the music.\n"
	"\n"
	"    The next option controls flashing.  When Blursk detects\n"
	"    a beat, it can brighten the whole image, invert the\n"
	"    whole image, or add a bitmap image (from the same pool\n"
	"    or images as the \"blur stencil\" option).  If this option\n"
	"    is set to \"Random flash\", then it will choose a bitmap\n"
	"    randomly; except if the stencil option is also random,\n"
	"    then blursk will use the stencil bitmap for flashing.\n"
	"\n"
	"    The effect option is next.  The \"Normal effect\" converts\n"
	"    the pixel values to colors directly.  The \"Bump effect\"\n"
	"    interprets the pixel values as heights, and chooses a\n"
	"    color based on height differences to achieve a cheap\n"
	"    3D effect.  The \"Anti-fade effect\" cycles the colormap\n"
	"    at exactly the same speed as fading, so that pixels tend\n"
	"    to remain the same color as they fade, so you see mostly\n"
	"    the blurring and motion but not the fading.  The \"Ripple\n"
	"    effect\" causes the fade to be subtly nonlinear.\n"
	"\n"
	"    The \"floaters\" option offers a way to spice up some\n"
	"    of the more sedate configurations.  \"Dots\" causes random\n"
	"    flashing dots to be added to the image.  The other values\n"
	"    add persistent dots which follow the blur motion, leaving\n"
	"    trails.\n"
	"\n"
	"=============================================================\n"
	"ADVANCED OPTIONS\n"
	"-------------------------------------------------------------\n"
	"\n"
	"MISCELLANY\n"
	"    The \"CPU speed\" option gives you a simple way to affect\n"
	"    the CPU load, by changing the image resolution.  The\n"
	"    \"Fast CPU\" setting uses the full resolution of the\n"
	"    window.  \"Medium CPU\" reduces the horizontal resolution\n"
	"    by half, and then interpolates points to expand the\n"
	"    image to fill the window.  The blur motions will be\n"
	"    distorted, but it should still look interesting.  The\n"
	"    \"Slow CPU\" setting reduces both vertical and horizontal\n"
	"    resolution.\n"
	"\n"
	"    The \"Show window title\" option causes the Blursk window\n"
	"    to be displayed as a normal application window, with a\n"
	"    title bar.  Normally it is displayed without a title\n"
	"    bar, like a dialog, but some window managers don't let\n"
	"    dialogs choose their own position.  This option is\n"
	"    normally off; if Blursk is unable to create its window\n"
	"    where you normally want it, then try turning this\n"
	"    option on.\n"
	"\n"
	"    The \"Show info\" option causes the track number and song\n"
	"    title to be shown in the image.\n"
	"\n"
	"BEAT SENSITIVITY\n"
	"    The beat sensitivity slider affects the beat sensor,\n"
	"    which is used by the \"hue on beats\", \"blur on beats\",\n"
	"    and \"flash\" features.  Moving the slider to the left\n"
	"    makes it less sensitive; moving it right increases\n"
	"    sensitivity.  Sadly, this isn't nearly as effective as\n"
	"    I'd hoped.\n"
	"\n"
	"FULL SCREEN\n"
	"    Blursk supports full-screen displays in a variety of\n"
	"    ways, each with their own quirks.  The exact list of\n"
	"    possible values depends on the libraries that were\n"
	"    available when blursk was configured and compiled, so\n"
	"    some of the following methods might not be available\n"
	"    to you.\n"
	"\n"
	"        \"Disabled\"      \n"
	"            Full-screen mode isn't used.\n"
	"\n"
	"        \"Use XMMS\"\n"
	"            Full-screen is supported via the XMMS full-\n"
	"            screen library functions.  This only works\n"
	"            on XFree86, and only with recent versions of\n"
	"            XMMS.  It works by temporarily setting your\n"
	"            video card to its lowest supported resolution,\n"
	"            and then resizing & repositioning the Blursk\n"
	"            window to fill visible part of the desktop.\n"
	"\n"
	"        \"Use XV\"\n"
	"            Full-screen is supported via the \"XV\" video\n"
	"            extension (also known as \"XVideo\").  To find\n"
	"            out if your X server supports XV, run the\n"
	"            \"xdpyinfo\" program and look for \"XVideo\" in\n"
	"            the list of extensions.  XV depends on special\n"
	"            hardware to scale a small image up to fill the\n"
	"            screen.\n"
	"\n"
	"        \"Use XV doubled\"\n"
	"            This also uses the \"XV\" video extension, but\n"
	"            in a slightly different way.  It tries to\n"
	"            avoid color artifacts that \"Use XV\" can\n"
	"            create where different colors meet, by\n"
	"            doubling the vertical & horizontal resolution.\n"
	"            This may interfere with the normal smoothing\n"
	"            function of your video card, and it will\n"
	"            probably be slower since the image is four\n"
	"            times larger.\n"
	"\n"
	"    The \"Shared memory\" flag only affects the \"Use XV\"\n"
	"    and \"Use XV doubled\" methods.  It causes Blursk to\n"
	"    use the shared memory versions of the XV imaging\n"
	"    functions.  Shared memory is faster than the normal\n"
	"    functions, but it doesn't work over a network.\n"
	"\n"
	"    The \"Alternative YUV\" flag only affects the \"Use XV\"\n"
	"    and \"Use XV doubled\" methods.  There are many ways\n"
	"    to convert RGB colors used by X-windows into YUV\n"
	"    colors used by XVideo.  By default, Blursk uses\n"
	"    YCbCr-601, but setting this flag will make it use\n"
	"    YCbCr-709.  Use whichever looks better on your\n"
	"    system.\n"
	"\n"
	"    The \"In root window\" flag is cool.  For the \"Use XV\"\n"
	"    and \"Use XV doubled\" methods, this flag causes Blursk\n"
	"    to send the image to the display's root window.  The\n"
	"    Blursk image then shows as animated wallpaper!\n"
	"    IMPORTANT NOTE: To revert to windowed mode, click the\n"
	"    [Blursk] button that appears where the Blursk window\n"
	"    used to be.\n"
	"\n"
	"    The \"Mask out edges\" flag can be used to blacken the\n"
	"    right and bottom edges.  Sometimes XV doesn't display\n"
	"    those pixels correctly, which can be distracting.\n"
	"    If this happens to you, then turn on this option.\n"
	"\n"
	"    The \"Revert to window at end\" flag tells Blursk to\n"
	"    switch back to window mode whenever the sound ends\n"
	"    -- usually at the end of an album.\n"
	"\n"
	"=============================================================\n"
	"KNOWN BUGS\n"
	"-------------------------------------------------------------\n"
	"\n"
	"KDE's window manager doesn't support the \"In root window\"\n"
	"option.  Also, the \"Show window title\" option should be\n"
	"left off; otherwise KDE will always move the window to\n"
	"the upper left corner when you exit full-screen mode.\n"
	"\n"
	"Gnome's Sawfish window manager has its own quirks.  The\n"
	"\"Show window title\" must be turned on, or the Blursk\n"
	"window will appear in a goofy place.\n"
	"\n"
	"Many window managers have trouble with the \"Use XMMS\"\n"
	"full-screen method.  Some WMs don't center the full-screen\n"
	"image, so you see stripes from other windows around two\n"
	"of the edges.  Some loose window focus, so the keystrokes\n"
	"aren't detected, and the only way to switch back to\n"
	"windowed mode is via the left mouse button.\n"
;
