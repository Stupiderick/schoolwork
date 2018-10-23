#include "scene.h"

Scene::Scene(int theMax) {
  max = theMax;
  myScene = new Image * [max];
  for (int i = 0; i < theMax; i++)
    myScene[i] = NULL;
  coordOfX = new int[max];
  coordOfY = new int[max];
  for (int i = 0; i < max; i++) {
    coordOfX[i] = 0;
    coordOfY[i] = 0;
  }
}

Scene::~Scene() {
  clear();
}

void Scene::clear() {
  for (int i = 0; i < max; i++) {
    if (myScene[i] != NULL)
      delete myScene[i];
    myScene[i] = NULL;
  }

  delete[] myScene;
  myScene = NULL;

  if (coordOfX != NULL)
    delete[] coordOfX;
  coordOfX = NULL;

  if (coordOfY != NULL)
    delete[] coordOfY;
  coordOfY = NULL;
}

Scene::Scene(const Scene & source) {
  copy(source);
}

const Scene & Scene::operator=(const Scene & source) {
  if (this != &source) {
    clear();
    copy(source);
  }
  return *this;
}

void Scene::copy(const Scene & source) {
  max = source.max;

  coordOfX = new int[max];
  coordOfY = new int[max];
  myScene = new Image * [max];

  for (int i = 0; i < max; i++) {
    coordOfX[i] = 0;
    coordOfY[i] = 0;
    myScene[i] = NULL;
  }

  for (int i = 0; i < max; i++)
    coordOfX[i] = source.coordOfX[i];

  for (int i = 0; i < max; i++)
    coordOfY[i] = source.coordOfY[i];

  for (int i = 0; i < max; i++) {
    if (source.myScene[i] != NULL) {
      myScene[i] = new Image(*source.myScene[i]);
    } else {
      source.myScene[i] = NULL;
    }
  }
}

void Scene::changemaxlayers(int newmax) {
  if (newmax < max) {
    for (int i = newmax - 1; i < max; i++) {
      if (myScene[i] != NULL) {
        cout << "invaild newmax" << endl;
        return;
      }
    }
  } else if (newmax == max) {
    return;
  }

  Image ** myTempScene = new Image * [newmax];
  int * tempCoordOfX = new int [newmax];
  int * tempCoordOfY = new int [newmax];

  if (newmax > max) {

    for (int i = 0; i < newmax; i++) {
      myTempScene[i] = NULL;
      tempCoordOfX[i] = 0;
      tempCoordOfY[i] = 0;
    }

    for (int i = 0; i < max; i++) {
      if (myScene[i] != NULL) {
        myTempScene[i] = myScene[i];
        tempCoordOfX[i] = coordOfX[i];
        tempCoordOfY[i] = coordOfY[i];
      }
    }

    for (int i = 0; i < max; i++) {
      myScene[i] = NULL;
    }

  } else {
    for (int i = 0; i < newmax; i++) {
      myTempScene[i] = NULL;
      tempCoordOfX[i] = 0;
      tempCoordOfY[i] = 0;
    }

    for (int i = 0; i < newmax; i++) {
      if (myScene[i] != NULL) {
        myTempScene[i] = myScene[i];
        tempCoordOfX[i] = coordOfX[i];
        tempCoordOfY[i] = coordOfY[i];
      }
    }

    for (int i = 0; i < newmax; i++) {
      myScene[i] = NULL;
    }
  }

    delete[] myScene;
    delete[] coordOfX;
    delete[] coordOfY;

    myScene = myTempScene;
    myTempScene = NULL;

    coordOfX = tempCoordOfX;
    coordOfY = tempCoordOfY;
    tempCoordOfX = NULL;
    tempCoordOfY = NULL;

    max = newmax;
}

void Scene::addpicture(const char * FileName, int index, int x, int y) {
  if ((index > max - 1) || (index < 0)) {
    cout << "index out of bounds" << endl;
    return;
  } else {
    Image tempImage(FileName);

    if (myScene[index] == NULL) {
      myScene[index] = new Image;
    }

    *(myScene[index]) = tempImage;
    coordOfX[index] = x;
    coordOfY[index] = y;
  }
}

void Scene::changelayer(int index, int newindex) {
  if (index != newindex) {
    if ((newindex > max - 1) || (newindex < 0))
      cout << "invaild index" << endl;
    else if ((index > max - 1) || (index < 0)) {
      cout << "invaild index" << endl;
    } else {
      if (myScene[newindex] != NULL)
        delete myScene[newindex];

      myScene[newindex] = myScene[index];
      coordOfX[newindex] = coordOfX[index];
      coordOfY[newindex] = coordOfY[index];
      myScene[index] = NULL;
    }
  }
}

void Scene::translate(int index, int xcoord, int ycoord) {
  if ((index > max - 1) || (index < 0)) {
    cout << "invaild index" << endl;
  } else if (myScene[index] == NULL) {
    cout << "invaild index" << endl;
  } else {
    coordOfX[index] = xcoord;
    coordOfY[index] = ycoord;
  }
}

void Scene::deletepicture(int index) {
  if ((index > max - 1) || (index < 0)) {
    cout << "invaild index" << endl;
  } else if (myScene[index] == NULL) {
    cout << "invaild index" << endl;
  } else {
    delete myScene[index];
    myScene[index] = NULL;
  }
}

Image * Scene::getpicture(int index) const {
  if ((index > max - 1) || (index < 0)) {
    cout << "invaild index" << endl;

    return NULL;
  } else {
    Image * ptr = myScene[index];

    return ptr;
  }
}

Image Scene::drawscene() const {
  size_t requiredWidth = 0;
  size_t requiredHeight = 0;

  for (int i = 0; i < max; i++) {
    if (myScene[i] != NULL) {
      size_t tempRequiredWidth = myScene[i]->width() + (size_t)coordOfX[i];
      size_t tempRequiredHeight = myScene[i]->height() + (size_t)coordOfY[i];

      if (tempRequiredHeight > requiredHeight)
        requiredHeight = tempRequiredHeight;

      if (tempRequiredWidth > requiredWidth)
        requiredWidth = tempRequiredWidth;
    }
  }

  Image myNewImage(requiredWidth, requiredHeight);

  for (int i = 0; i < max; i++) {
    if (myScene[i] != NULL) {
      size_t localWidth = myScene[i]->width();
      size_t localHeight = myScene[i]->height();

      for (size_t column = 0; column < localWidth; column++) {
        for (size_t row = 0; row < localHeight; row++) {
          myNewImage((size_t)coordOfX[i] + column, (size_t)coordOfY[i] + row)->red = (*myScene[i])(column, row)->red;
          myNewImage((size_t)coordOfX[i] + column, (size_t)coordOfY[i] + row)->green = (*myScene[i])(column, row)->green;
          myNewImage((size_t)coordOfX[i] + column, (size_t)coordOfY[i] + row)->blue = (*myScene[i])(column, row)->blue;
        }
      }
    }
  }

return myNewImage;
}
