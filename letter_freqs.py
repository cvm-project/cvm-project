import sys, os

sys.path.append(os.getcwd())

from blaze.blaze_context import BlazeContext
from timeit import default_timer as timer

start = timer()
bc = BlazeContext()
tf = bc.text_file("blaze/example_data/xaa")
# tf = bc.text_file("/home/sabir/gutemberg.txt")
words = tf.flat_map(lambda line: line.split(" ")).map(lambda w: w.lower())

letter_counts = words.flat_map(lambda w: list(w)). \
    map(lambda l: (l.lower(), 1))\
    .reduce_by_key(lambda v1, v2: v1+v2)\
    .sort(key=lambda t: t[1], reverse=True).group_by_key()

total_letters = letter_counts.map(lambda w: (1, w[1])).reduce_by_key(lambda v1, v2: v1+v2).collect()[0][1]

freqs = letter_counts.map(lambda w: (w[0], "{0:.2f}".format(float(w[1])/(float(total_letters))*100, 2)+'%')).collect()

end = timer()
print(freqs)
print("Time elapsed %f" % (end - start))
