import gym
import pickle
import numpy as np
import sys
import codecs

env = gym.make('FrozenLake-v0')
s = env.reset()

with open("policy.pkl", 'rb') as file:
    policy  = pickle.load(file)

for i in range(1000):
    
    a = np.random.choice(range(env.action_space.n), p=policy[s, :])
    obs, r, done, info = env.step(a)
    s = obs

    rendering = env.render('ansi')
    # print(rendering, file=codecs.open("./video/step_" + str(i) + ".txt", 'w', encoding = 'utf8'))
    print(rendering)
    if done:
        break
env.close()
