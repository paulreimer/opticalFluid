CvPoint2D32f* swp_f = cornersPrev;
cornersPrev = cornersImg;
cornersImg = swp_f;

cvCalcOpticalFlowLK(grayImg.getCvImage(),
                    grayPrev.getCvImage(),
                    cvSize(3,3),
                    velx.getCvImage(),
                    vely.getCvImage()
                    );
dvX = velx;
dvY = vely;
dv += dvX;
dv += dvY;
dv.threshold(threshold);
dvX.threshold(dv_threshold);
dvY.threshold(dv_threshold);
int x,y;
float dx, dy;
float dx_total, dy_total;
for(y=0; y<VIDEO_HEIGHT; y++) {
  for(x=0; x<VIDEO_WIDTH; x++) {
    dx = ((float*)velx.getCvImage()->imageData)[y*velx.getCvImage()->width + x];
    dy = ((float*)vely.getCvImage()->imageData)[y*vely.getCvImage()->width + x];
    
    if (fabs(dx) > dv_threshold || fabs(dy) > dv_threshold) {
      fluidSolver.addForceAtPos((float)x/VIDEO_WIDTH, (float)y/VIDEO_HEIGHT, dx/dv_scale, dy/dv_scale);
//          printf("x: %f, y: %f - dvx: %f, dvy: %f\n", (float)x/VIDEO_WIDTH, (float)y/VIDEO_HEIGHT, dx/dv_scale, dy/dv_scale);
    }
  }
}

// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
// also, find holes is set to true so we will get interior contours as well....
//contourFinder.findContours(grayDiff, 20, (VIDEO_WIDTH*VIDEO_HEIGHT)/3, 10, true); // find holes
