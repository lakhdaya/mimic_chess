#!/usr/bin/env python
# coding: utf-8

# In[30]:


def convert(l):
    import numpy as np
    return np.int32([int(i) for i in l[1:-1].split(',')])

def get_output(value):
    return value


# In[31]:



def get_accuracy(history):
    import matplotlib.pyplot as plt
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


# In[35]:


def create_model(player):
    import pandas as pd
    import numpy as np
    import tensorflow as tf
    from tqdm import tqdm
    import tensorflow.keras
    from keras import layers, models
    from tensorflow.keras import optimizers
    from sklearn.model_selection import train_test_split
    from tensorflow.keras.utils import to_categorical

    from keras import layers, models
    from tensorflow.keras import optimizers

    games = pd.read_csv("mimic/data/" + player + ".csv")
    X = np.array([ np.array(convert(board)) for board in tqdm(games["bitboard"])])
    Y = np.array(games["winrate"])
    X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.3)
    print(X_train.shape)
    input_dim = (773, )

    model = models.Sequential()

    model.add(layers.Dense(2048, activation='relu', input_shape = input_dim))
    model.add(layers.Dropout(0.3))
    model.add(layers.Dense(1024, activation='relu'))
    model.add(layers.Dropout(0.3))
    model.add(layers.Dense(1024, activation='relu'))
    model.add(layers.Dense(512, activation='relu'))
    model.add(layers.Dense(512, activation='relu'))


    model.add(layers.Dense(1, activation='sigmoid'))

    model.compile(loss='mean_squared_error', optimizer=optimizers.Adam(learning_rate=1e-4), metrics=['acc'])
    history = model.fit(
      X_train, Y_train,
      epochs=20,
      steps_per_epoch = X_train.shape[0]/20,
      validation_split=0.1)
    get_accuracy(history)
    Y_pred = model.predict(X_test).reshape(X_test.shape[0])
    print("accuracy test: ", np.mean((Y_pred-Y_test)**2))
    model.save("mimic/models/" + player + "_2")
 