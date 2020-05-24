#include <stdio.h>
#include <assert.h>

#include "_arg_parser.h"
#include "_glCanvas.h"
#include "_spline_parser.h"

double M_PI = 3.14159265358979323846;

int main(int argc, char *argv[]) {

    // parse the command line arguments & input file
    ArgParser *args = new ArgParser(argc, argv);
    SplineParser *splines = new SplineParser(args->input_file);

    // launch curve editor!
    if (args->gui) {
        GLCanvas glcanvas;
        glcanvas.initialize(args, splines);
        // this never returns...
    }

    // output as required
    splines->SaveBezier(args);
    splines->SaveBSpline(args);
    splines->SaveTriangles(args);

    // cleanup
    delete args;
    delete splines;
    return 0;
}

// ====================================================================
// ====================================================================

