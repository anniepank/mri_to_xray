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

float interpolate(float x, float a, float b) {
    return (b - a) * x + a;
}

int main(int argc, char *argv[]) {

    nifti_image *nim_input = NULL, *nim_output = NULL;
    char *fin = NULL, *fout = NULL;
    int cutoff, ac, disp_float_eg = 0;

    fin = argv[2];
    // read input dataset, including data

    nim_input = nifti_image_read(fin, 1);

    int dim[8] = {2, 512, 512, 1, 1, 0, 0, 0};
    nim_output = nifti_make_new_nim(dim, DT_SIGNED_SHORT, true);
    nim_output->dx = 1;
    nim_output->dy = 1;
    nim_output->dz = 1;
    nim_output->dv = 0;
    nim_output->du = 0;
    nim_output->dt = 0;
    nim_output->dw = 0;


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
    png::image<png::rgb_pixel> image(512, 512);

    int _min = -1024;
    int _max = 2438;

    for (size_t y = 0; y < image.get_height(); ++y) {
        for (size_t x = 0; x < image.get_width(); ++x) {
            float result = 0;
            float rayStrength = 1;
            for (size_t v = 0; v < 512; v++) {

                int value1, value2;

                if ((nxy * (int16_t) ceil(y / (nim_input->dz / nim_input->dx))) +
                    nx * (int16_t) ceil(v / (nim_input->dy / nim_input->dx)) + x >= 512 * 512 * 123 ) {
                    value1 = 0;
                    value2 = 0;
                } else {

                    value1 = *(nim_input_data +
                               (nxy * (int16_t) floor(y / (nim_input->dz / nim_input->dx))) +
                               nx * (int16_t) floor(v / (nim_input->dy / nim_input->dx)) + x);

                    value2 = *(nim_input_data +
                               (nxy * (int16_t) (ceil(y / (nim_input->dz / nim_input->dx)))) +
                               nx * (int16_t) floor(v / (nim_input->dy / nim_input->dx)) + x);

                }

                float interpX = y * (nim_input->dz / nim_input->dx);

                float interpolated = interpolate(interpX - floor(interpX), value1, value2);
                float value = interpolated;

                if (value <= -1000) {
                    value = -1000;
                }

                float normalized = (value - _min) / (_max - _min);
                normalized *= normalized ;
                rayStrength = rayStrength * (1 - normalized / 40);
                // result = result * (1 - opacity) + 1 *  opacity;
            }

            result = 1 - rayStrength;
            int ir = (int) (result * 256);

            image[511 - y][x] = png::rgb_pixel(ir, ir, ir);
            ((int16_t*)nim_output->data)[512 * y + x] = (int16_t) ir;

        }
    }

    nim_output->fname = (char *) "output.nii";
    nim_output->iname = (char *) "output.nii";

    nifti_image_write(nim_output);

    image.write("rgb.png");

    return 0;
}

/*
 * value = std::max(-1000, value);
                float fv = ((float)value + 1000) / 2300;
                fv = std::min(fv, (float)1.0);
                float opacity = fv / 500;

                result = result * (1 - opacity) + 1 * opacity;
                */