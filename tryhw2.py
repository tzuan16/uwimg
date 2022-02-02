from uwimg import *
im = load_image("data/dog.jpg")
res = colorize_sobel(im)
feature_normalize(res)
save_image(res, "magnitude")