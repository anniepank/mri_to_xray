//compile with with "make My_nii_read"
//execute with ./My_nii_read -input input_example.nii -output output.nii -cutoff 3

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <gsl/gsl_statistics_double.h>

#include "inst/include/RNifti.h"
#include "inst/include/niftilib/nifti1_io.h"
#include <png++/png.hpp>

using namespace std;

int main(int argc, char *argv[]) {

    nifti_image *nim_input = NULL;
    char *fin = NULL, *fout = NULL;
    int cutoff, ac, disp_float_eg = 0;

    fin = argv[2];
    // read input dataset, including data

    nim_input = nifti_image_read(fin, 1);
    if (!nim_input) {
        fprintf(stderr, "** failed to read NIfTI image from '%s'\n", fin);
        return 2;
    }


    // get dimsions of input
    int sizeSlice = nim_input->ny;
    int sizePhase = nim_input->nx;
    int sizeRead = nim_input->nz;
    int nrep = 1;
    int nx = nim_input->nx;
    int nxy = nim_input->nx * nim_input->ny;
    int nxyz = nim_input->nx * nim_input->ny * nim_input->nz;

    cout << sizeSlice << " slices    " << sizePhase << " PhaseSteps     " << sizeRead << " Read steps    " << nrep
         << " timesteps " << endl;

    // get access to data of nim_input
    auto nim_input_data = (int16_t *) nim_input->data;
/*
 for (int islice = 0; islice < sizeSlice; ++islice) {
        for (int iy = 0; iy < sizePhase; ++iy) {
            for (int ix = 0; ix < sizeRead; ++ix) {
                if (*(nim_input_data + nxy * islice + nx * ix + iy)  > 0)
                auto a = *(nim_input_data + nxy * islice + nx * ix + iy);
                        /* nim_input_data + nxyz * timestep + nxy * islice + nx * ix + iy *
            }
        }
    }
*/

    png::image<png::rgb_pixel> image(512, 512);
    float min = 0;
    float max = -2000;
    for (size_t y = 0; y < image.get_height(); ++y) {
        for (size_t x = 0; x < image.get_width(); ++x) {
            float result = 0;

            for (size_t z = 0; z < 512; z++) {
                int value =  *(nim_input_data + nxy * y * (123 / 512) + nx * z + x);
                //cout << value << endl;
                if (value > -100) {
                    cout << value << endl;
                }
                if (value > max) {
                    max = value;
                }
                if (value < min) {
                    min = value;
                }


                value = std::max(-1000, value);
                float fv = ((float)value + 1000) / 2300;
                fv = std::min(fv, (float)1.0);
                float opacity = fv / 300;

                result = result * (1 - opacity) + 1 * opacity;
            }

            int ir = (int)(result);
            if (result != 0) {
                cout << result << endl;
            }
            image[y][x] = png::rgb_pixel(ir, ir, ir);
            // non-checking equivalent of image.set_pixel(x, y, ...);
        }
    }

    cout << min << " " << max << endl;
//    image.write("my_rgb.png");

    return 0;
}




/* for (size_t v = 0; v < 512; v++) {
                int value = *(nim_input_data + nxy * (y / 5) + nx * v + x);
                value = std::max(-1000, value);
                float fv = ((float)value + 1000) / 2300;
                fv = std::min(fv, (float)1.0);
                float opacity = fv / 300;

                result = result * (1 - opacity) + 1 * opacity;

    }

            int ir = (int)(result * 256);
            image[511-y][x] = png::rgb_pixel(ir, ir, ir);
            // non-checking equivalent of image.set_pixel(x, y, ...);

 */