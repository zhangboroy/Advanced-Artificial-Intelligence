import pandas as pd

# the objective function
def objective(trial):
    # Import data
    data = pd.read_csv('sin.csv')

    # Dimensions of dataset
    n = data.shape[0]
    p = data.shape[1]

    # Make data a numpy array
    data = data.values

    import numpy as np

    # Training and test data
    train_start = 0
    train_end = int(np.floor(0.8*n))
    test_start = train_end
    test_end = n
    data_train = data[np.arange(train_start, train_end), :]
    data_test = data[np.arange(test_start, test_end), :]

    # Build X and Y
    X_train = data_train[:, 0]
    y_train = data_train[:, 1]
    X_test = data_test[:, 0]
    y_test = data_test[:, 1]

    X_train = (X_train - data[:,0].mean()) / data[:,0].std()
    y_train = (y_train - data[:,1].mean()) / data[:,1].std()
    X_test = (X_test - data[:,0].mean()) / data[:,0].std()
    y_test = (y_test - data[:,1].mean()) / data[:,1].std()

    X_train = X_train.reshape(-1,1)
    y_train = y_train.reshape(-1,1)
    X_test = X_test.reshape(-1,1)
    y_test = y_test.reshape(-1,1)

    import tensorflow as tf

    input_dim = 1

    # Placeholder
    X = tf.placeholder(dtype=tf.float32, shape=[None, input_dim])
    Y = tf.placeholder(dtype=tf.float32, shape=[None, input_dim])

    # Initializers
    sigma = 1
    weight_initializer = tf.variance_scaling_initializer(mode="fan_avg", distribution="uniform", scale=sigma)  #[-sqrt(3 * scale / n), sqrt(3 * scale / n)]
    bias_initializer = tf.zeros_initializer()

    # Create layers
    # number of intermediate layers could be 0 or 1
    n_layers = trial.suggest_int('n_layers', 0, 1)

    # the input layer
    n_units1 = input_dim #number of units in the previous layer
    n_units2 = int(trial.suggest_loguniform('n_units_l{}'.format(0), 4, 256)) #number of units in the current layer, loguniform distribution from 4 to 256
    w = tf.Variable(weight_initializer([input_dim, n_units2]))
    b = tf.Variable(bias_initializer([n_units2]))
    layers = [tf.nn.sigmoid(tf.add(tf.matmul(X, w), b))]

    # the intermediate layers
    for i in range(n_layers):
        n_units1 = n_units2
        n_units2 = int(trial.suggest_loguniform('n_units_l{}'.format(i+1), 4, n_units2/2))
        w = tf.Variable(weight_initializer([n_units1, n_units2]))
        b = tf.Variable(bias_initializer([n_units2]))
        layers.append(tf.nn.sigmoid(tf.add(tf.matmul(layers[i], w), b)))

    # the output layer
    w = tf.Variable(weight_initializer([n_units2, 1]))
    b = tf.Variable(bias_initializer([1]))
    layers.append(tf.add(tf.matmul(layers[-1], w), b))

    # Loss function
    mse = tf.reduce_mean(tf.squared_difference(layers[-1], Y))

    # Optimizer
    opt = tf.train.AdamOptimizer().minimize(mse)

    # Make Session
    sess = tf.Session()

    # Run initializer
    sess.run(tf.global_variables_initializer())

    epochs = 10000

    for e in range(epochs):
        # Run optimizer
        sess.run(opt, feed_dict={X: X_train, Y: y_train})

    mse_final = sess.run(mse, feed_dict={X: X_train, Y: y_train})
    return mse_final

if __name__ == '__main__':
    import optuna
    study = optuna.create_study()
    # try 100 times
    study.optimize(objective, n_trials=100)
