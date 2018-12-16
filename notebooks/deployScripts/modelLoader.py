from skimage import io
from argparse import ArgumentParser
import sys 
import os

import pickle
import numpy as np
import cv2
import json

def GetCommandLineArgs():
  parser = ArgumentParser()
  parser.add_argument("-u", "--url", dest="url",
                  help="URL of the image to be tested.[mandatory]",
                  metavar="URL")
  parser.add_argument("-s", "--sess", dest="sessionId",
                  help="Session ID of the http request.[mandatory]",
                  metavar="Session ID")
  parser.add_argument("-i", "--index", dest="index",
                  help="Model Id index.[mandatory]",
                  metavar="Index of Model ID")

  args = parser.parse_args()

  url = ''
  sessId = ''
  modelIndex = -1

  if not args.url:
    print('Invalid Command aruguments.')
    print('Usage:') 
    print('"python modelLoader.py -h" for more details')
    exit()
  else:
    url = args.url

  if not args.sessionId:
    print('Invalid Command aruguments.')
    print('Usage:') 
    print('"python modelLoader.py -h" for more details')
    exit()
  else:
    sessId = args.sessionId

  if not args.index:
    print('Invalid Command aruguments.')
    print('Usage:') 
    print('"python modelLoader.py -h" for more details')
    exit()
  else:
    modelIndex = args.index

  return sessId, url, modelIndex 

def predict(modelFile,x_test):
  model = pickle.load(open(modelFile, 'rb'))
  predict = model.predict(x_test)
  return predict

def processImage(imgFile):
  #color information is not required in this model.
  #hence read the image file as grayscale image.
  img = cv2.imread(imgFile, cv2.IMREAD_GRAYSCALE)
  retArr = np.array([])
  if img is None:
    print('Cannot open file {}'.format(imgFile))
    return retArr
  else :
    #print('Image is fine')
    #Binarize the image.
    thresh = 0
    maxValue = 255
    th, dst = cv2.threshold(img, thresh, maxValue, cv2.THRESH_BINARY)
    #re-size the image to 28*28 as that is the dimention 
    #our MNIST model is trained against.
    compatibleSize = (28,28)
    re_size_img = cv2.resize(dst, compatibleSize)
    #Flatten the image in to 1d array
    img_flat = np.array(re_size_img.flatten())
    retArr = np.reshape(img_flat,(1,28*28))
    return retArr


def StoreImage(url,saveFile):
  image = io.imread(url)
  cv2.imwrite(saveFile, image)


def CreateDirectory(path):
  os.makedirs(path, exist_ok=True)


def main(argv):
  sessId, url, index = GetCommandLineArgs()
  print('session = {} url = {} modelIndex = {}'.format(sessId, url, index))
  
  '''
  Create a directory structure to store the image file
  and required meta data for each session.
  Ex: /tmp/onfidon/<sessionId>
  '''
  path = '/tmp/onfido/'+sessId
  CreateDirectory(path)
  saveImgFile = path +'/number.jpg'
  respJsonFile = path + '/response.json'
  
  print('path imgfile = {}'.format(saveImgFile))
  print('path respJsonFile = {}'.format(respJsonFile))

  '''
  Saving an image is an over head and computationally expensive operation.
  To keep the logic simple we are making this compromise.
  Images availabe in the internet are of different format say,
  RGB,BGR,GRASCALE,HUV 3 or 4 channel etc. Trying
  to put a code to handle all these formats of data will blowup the code  complexity.
  Hence irrespective of the image format, we just
  store it and read it as grayscale image. As MNIST data set or text
  image data doesn't carry any information with the color. 
  So reading the image as grayscale format is perfectly logical.
  '''
  response = {}
  
  response['SessionId'] = sessId

  StoreImage(url,saveImgFile)
  arr = processImage(saveImgFile)
  
  pklFiles = ['logistic_regression_model_default.pkl', 'knn_model_K_3.pkl',
              'cnn_model_tobe_put_in']

  index = int(index)

  if index <= 2 :
    modelFile = '/opt/onfido/models/' + pklFiles[index]

    if arr.size == 0:
      print('Problem with the image file : {}'.format(saveImgFile))
      response['Status'] = '500 OK'
      response['Prediction'] = -1
    else:
      res = predict(modelFile, arr)
      print ('prediction is = {}'.format(res))
      response['Status'] = '200 OK'
      response['Prediction'] = int(res)

  else:
      response['Status'] = '300 OK'
      response['Prediction'] = -1

  with open(respJsonFile, "w") as write_file:
    json.dump(response, write_file)


if __name__ == "__main__":
  main(sys.argv)
