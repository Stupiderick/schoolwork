import numpy as np
import matplotlib.pyplot as plt

x = np.arange(0.0, 100.0, 0.01)
y = []
G = 9.81

for i in x:
    if i < 3.0:
        y.append(0.3 * G)
    elif i > 5.0:
        y.append(3 * G / i)
    else:
        y.append(0.15 * G * (i - 1))

plt.plot(x, y)
plt.grid(which='major', linestyle=':', axis='both')
plt.xlim(0, 100)
plt.ylim(-1, 7)
plt.xlabel('T (sec)')
plt.ylabel('Sa (m/s^2)')
plt.title('T vs Sa')
plt.annotate('Maximum', xy=(5, 3*G/5), xytext=(20, 5),
            arrowprops=dict(facecolor='black', shrink=0.1),
            )
plt.xticks(np.arange(0, 105, 5))
plt.yticks(np.arange(-1., 7.5, 0.5))
plt.show()
