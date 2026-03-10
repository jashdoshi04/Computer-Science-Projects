"""Support Vector Machine (SVM) model."""

import numpy as np


class SVM:
    def __init__(self, n_class: int, lr: float, epochs: int, reg_const: float):
        """Initialize a new classifier.

        Parameters:
            n_class: the number of classes
            lr: the learning rate
            epochs: the number of epochs to train for
            reg_const: the regularization constant
        """
        self.w = None  # TODO: change this
        self.lr = lr
        self.epochs = epochs
        self.reg_const = reg_const
        self.n_class = n_class

    def calc_gradient(self, X_train: np.ndarray, y_train: np.ndarray) -> np.ndarray:
        """Calculate gradient of the svm hinge loss.

        Inputs have dimension D, there are C classes, and we operate on
        mini-batches of N examples.

        Parameters:
            X_train: a numpy array of shape (N, D) containing a mini-batch
                of data
            y_train: a numpy array of shape (N,) containing training labels;
                y[i] = c means that X[i] has label c, where 0 <= c < C
            N: the number of examples in the mini-batch
        Returns:
            the gradient with respect to weights w; an array of the same shape
                as w
        """
        # TODO: implement me
         
        n = X_train.shape[0]
        grad = (self.reg_const / n) * self.w
        for i in range(n):
            xi = X_train[i]
            yi = y_train[i]
            for k in range(self.n_class):
                if k == yi:
                    continue
                margin = np.dot(self.w[k], xi) - np.dot(self.w[yi], xi) + 1
                if margin > 0:
                    grad[k] += xi / n
                    grad[yi] -= xi / n
                
        return grad
                        
                        
        
        

    def train(self, X_train: np.ndarray, y_train: np.ndarray):
        """Train the classifier.

        Hint: operate on mini-batches of data for SGD.
        - Initialize self.w as a matrix with random values sampled uniformly from [-1, 1)
        and scaled by 0.01. This scaling prevents overly large initial weights,
        which can adversely affect training.

        Parameters:
            X_train: a numpy array of shape (N, D) containing training data;
                N examples with D dimensions
            y_train: a numpy array of shape (N,) containing training labels
        """
        # Initialize weights: random values from [-1, 1) scaled by 0.01
        np.random.seed(30)  # For reproducibility
        self.w = np.random.uniform(-1, 1, (self.n_class, X_train.shape[1])) * 0.01
        
        # Perform SGD for specified number of epochs
        for epoch in range(self.epochs):
            indices = np.random.permutation(X_train.shape[0])
            for i in range(0, len(indices), 64):
                batch_indices = indices[i:i+64]  # Mini-batch of size 500
                X_batch = X_train[batch_indices]
                y_batch = y_train[batch_indices]
                grad = self.calc_gradient(X_batch, y_batch)
                self.w -= self.lr * grad

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
        return np.argmax(X_test @ self.w.T, axis=1)
