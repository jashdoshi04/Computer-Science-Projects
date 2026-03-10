"""Softmax model."""

import numpy as np


class Softmax:
    def __init__(self, n_class: int, lr: float, epochs: int, reg_const: float, decay_rate: float):
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
        self.decay = decay_rate

    def calc_gradient(self, X_train, y_train):
        N, D = X_train.shape
        scores = X_train @ self.w.T                      # (N, C)
        scores -= np.max(scores, axis=1, keepdims=True)  # stability
        exp_scores = np.exp(scores)
        probs = exp_scores / np.sum(exp_scores, axis=1, keepdims=True)  # (N, C)
        y_onehot = np.zeros_like(probs)
        y_onehot[np.arange(N), y_train] = 1
        gradient = (probs - y_onehot).T @ X_train        # (C, D)
        gradient /= N
        gradient += self.reg_const * self.w

        return gradient


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
        # TODO: implement me
        N, D = X_train.shape
        # Initialize weights uniformly from [-1, 1) and scale by 0.01
        self.w = 0.01 * np.random.uniform(-1, 1, (self.n_class, D))
        batch_s = 128
        num_batch = N // batch_s
            
        for epoch in range(self.epochs):
            indices = np.random.permutation(N)
            X_shuffled = X_train[indices]
            y_shuffled = y_train[indices]
            
            for batch_idx in range(num_batch):
                start_idx = batch_idx * batch_s
                end_idx = min(start_idx + batch_s,N)
                
                X_batch = X_shuffled[start_idx:end_idx]
                y_batch = y_shuffled[start_idx:end_idx]

                gradient = self.calc_gradient(X_batch, y_batch)
                self.w -= self.lr * gradient
            self.lr *= np.exp(-epoch / self.decay)

            if N % batch_s != 0:
                X_batch = X_shuffled[num_batch * batch_s:]
                y_batch = y_shuffled[num_batch * batch_s:]
                
                gradient = self.calc_gradient(X_batch, y_batch)
                self.w -= self.lr * gradient

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
        scores = X_test @ self.w.T  # Shape: (N, n_class)
        predictions = np.argmax(scores, axis=1)  # Shape: (N,)
        
        return predictions
