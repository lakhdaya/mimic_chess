
import pandas as pd
import numpy as np
import tensorflow as tf
from tqdm.notebook import tqdm

NAME = "arlsen_magnus"
def convert(l):
    return np.float32([int(i) for i in l.split(',')[1:-1]])
def get_output(value):
    output = np.zeros(11)
    output[int(value*10)] = 1
    return output

import tensorflow.keras
from tqdm.notebook import tqdm
from keras import layers, models
from tensorflow.keras import optimizers
from sklearn.model_selection import train_test_split
from tensorflow.keras.utils import to_categorical
    
from keras import layers, models
from tensorflow.keras import optimizers
import matplotlib.pyplot as plt

def get_accuracy(history):
    acc = history.history['acc']
    val_acc = history.history['val_acc']
    loss = history.history['loss']
    val_loss = history.history['val_loss']

    epochs = range(len(acc))

    print(len(acc), len(val_acc))

    plt.plot(epochs, acc, 'b', label='Training acc')
    plt.plot(epochs, val_acc, 'm', label='Validation acc')
    plt.title('Training and validation accuracy')
    plt.legend()

    plt.figure()

    plt.plot(epochs, loss, 'b', label='Training loss')
    plt.plot(epochs, val_loss, 'm', label='Validation loss')
    plt.title('Training and validation loss')
    plt.legend()

    plt.show()

def create_model(games, player):
    games = pd.read_csv("mimic/data/" + player + ".csv")
    X = games["bitboard"]
    X = np.array([ np.array(convert(board)) for board in tqdm(games["bitboard"])])
    Y = np.array([get_output(wr) for wr in tqdm(games["winrate"])])
    X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.3)
    input_dim = (771,)

    model = models.Sequential()

    model.add(layers.Dropout(rate=0.3))
    model.add(layers.Dense(2048, activation='relu'))
    model.add(layers.Dropout(rate=0.3))

    model.add(layers.Dense(1024, activation='relu'))
    model.add(layers.Dropout(rate=0.3))

    model.add(layers.Defnse(1024, activation='relu'))
    model.add(layers.Dense(512, activation='relu'))

    model.add(layers.Dense(11, activation='softmax'))

    model.compile(loss='categorical_crossentropy', optimizer=optimizers.Adam(learning_rate=1e-4), metrics=['acc'])
    history = model.fit(
        X_train, Y_train,
        epochs=20,
        validation_split=0.1)
    get_accuracy(history)
    Y_pred = model.predict(X_test)
    pred = np.argmax(Y_pred, axis=1)
    test = np.argmax(Y_test, axis=1)
    print("accuracy test: ", np.sum(pred==test)/pred.shape[0]*100, "%")
    model.save("models/" + player)
