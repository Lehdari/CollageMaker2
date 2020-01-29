# CollageMaker2

CollageMaker2 is a software that creates collage images from source images.
This is achieved by minimizing error between current and target image by
transforming source images. Transformations are iteratively updated with
gradient descent method. Both error and gradient computations are done on
GPU with GLSL compute shaders.
