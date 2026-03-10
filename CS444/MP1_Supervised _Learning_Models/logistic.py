"""Logistic regression model."""

import numpy as np


class Logistic:
    def __init__(self, lr: float, epochs: int, threshold: float):
        """Initialize a new classifier.

        Parameters:
            lr: the learning rate
            epochs: the number of epochs to train for
        """
        self.w = None
        self.lr = lr
        self.epochs = epochs
        self.threshold = threshold

    def sigmoid(self, z: np.ndarray) -> np.ndarray:
        """Sigmoid function.

        Parameters:
            z: the input

        Returns:
            the sigmoid of the input
        """
        result = np.zeros_like(z)
        positive_values = z >= 0
        negative_values = z < 0
        result[positive_values] = 1 / (1 + np.exp(-z[positive_values]))
        exp_z = np.exp(z[negative_values])
        result[negative_values] = exp_z / (1 + exp_z)
       
        return result

    def train(self, X_train: np.ndarray, y_train: np.ndarray):
        """Train the classifier.

        - Use the logistic regression update rule as introduced in lecture.
        - Initialize self.w as a matrix with random values sampled uniformly from [-1, 1)
        and scaled by 0.01. 
        - This initialization prevents the weights from starting too large,
        which can cause saturation of the sigmoid function 

        Parameters:
            X_train: a numpy array of shape (N, D) containing training data;
                N examples with D dimensions
            y_train: a numpy array of shape (N,) containing training labels
        """
        n_samples, n_features = X_train.shape
        self.w = 0.01 * np.random.uniform(-1.0, 1.0, size=n_features)
        for epoch in range(self.epochs):
            for i in range(n_samples):
                x_i = X_train[i]     
                y_i = y_train[i]      
                score = np.dot(self.w, x_i)
                error_signal = self.sigmoid(-y_i * score)
                self.w += self.lr * error_signal * y_i * x_i

    def predict(self, X_test: np.ndarray) -> np.ndarray:
        """Use the trained weights to predict labels for test data points.

        Parameters:
            X_test: a numpy array of shape (N, D) containing testing data;
                N examples with D dimensions

        Returns:exce
            predicted labels for the data in X_test; a 1-dimensional array of
                length N, where each element is an integer giving the predicted
                class.
        """
        probs = self.sigmoid(X_test @ self.w)
        return (probs >= self.threshold).astype(int)
