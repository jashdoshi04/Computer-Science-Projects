"""Perceptron model."""

import numpy as np
import matplotlib.pyplot as plt

class Perceptron:
    def __init__(self, n_class: int, lr: float, epochs: int, decay_rate: float):
        """Initialize a new classifier.

        Parameters:
            n_class: the number of classes
            lr: the learning rate
            epochs: the number of epochs to train for
        """
        self.w = None  # TODO: change this
        self.lr = lr
        self.epochs = epochs
        self.n_class = n_class
        self.decay_rate = decay_rate    
        self.training_loss = []
        np.random.seed(30)
    def train(self, X_train: np.ndarray, y_train: np.ndarray):
        """Train the classifier.

        - Use the perceptron update rule as introduced in the Lecture.
        - Initialize self.w as a matrix with random values sampled uniformly from [-1, 1)
        and scaled by 0.01. This scaling prevents overly large initial weights,
        which can adversely affect training.

        Parameters:
            X_train: a number array of shape (N, D) containing training data;
                N examples with D dimensions
            y_train: a numpy array of shape (N,) containing training labels
        """
        # TODO: implement me
        y_train = y_train.astype(int).reshape(-1, 1)
        N = np.shape(X_train)[0]
        D = np.shape(X_train)[1]
        gen = np.random.RandomState(seed=30)
        self.w = 0.01 * gen.uniform(-1, 1, (D, self.n_class))
        eta = self.lr
        epochs = self.epochs
        
        # print(np.shape(y_train))
        # print(np.shape(self.w))
        # print(np.shape(X_train))
        # print(y_train.T @ (self.w) @ X_train)
        for i in range(epochs):
            # if _ % 100 == 0:
            
            #     eta *= 0.9
            eta = self.lr * np.exp(- i / self.decay_rate)
            n_errors = 0
            idx = np.random.permutation(N)
            for i in idx:
                xi = X_train[i].reshape(-1, 1)
                yi = y_train[i][0]
                y_pred = np.argmax(self.w.T @ xi)
                if yi != y_pred:   
                    self.w[:, yi] += eta * xi.flatten()
                    self.w[:, y_pred] -= eta * xi.flatten()
                    self.w[-1, yi] += eta
                    self.w[-1, y_pred] -= eta
                    n_errors += 1
                else:
                    continue
            self.training_loss.append(n_errors / N)
                        
    def predict(self, X_test: np.ndarray) -> np.ndarray:
        """Use the trained weights to predict labels for test data points.

        Parameters:
            X_test: a numpy array of shape (N, D) containing testing data;
                N examples with D dimensions

        Returns:
            predicted labels for the data in X_test; a 1-dimensional array of
                length N, where each element is an integer giving the predicted
                class.
        """
        # TODO: implement me
        return np.argmax(self.w.T @ X_test.T, axis=0)

    def plot_training_loss(self):
        plt.plot(self.training_loss, label = 'training loss')
        plt.xlabel('Epoch')
        plt.ylabel('Training Loss')
        plt.title('Perceptron Training Loss Curve')
        plt.legend()
        
        plt.show()