import sys, os

sys.path.append(os.getcwd())

from blaze.blaze_context import BlazeContext
from timeit import default_timer as timer

start = timer()
bc = BlazeContext()
tf1 = bc.text_file("blaze/example_data/xaa")
# tf1 = bc.text_file("blaze/example_data/nat.txt")
tf2 = bc.text_file("blaze/example_data/xab")
# tf2 = bc.text_file("blaze/example_data/nat.txt")

words_count1 = tf1.flat_map(lambda line: line.split(" "))\
    .map(lambda w: (w.lower(), 1))\
    .reduce_by_key(lambda v1, v2: v1+v2)

words_count2 = tf2.flat_map(lambda line: line.split(" "))\
    .map(lambda w: (w.lower(), 1))\
    .reduce_by_key(lambda v1, v2: v1+v2)

wc = words_count1.join(words_count2)\
    .map(lambda tupl: (tupl[0], sum(tupl[1])))\
    .sort(key=lambda w: w[1], reverse=True)

wc.cache()
wc.collect()
total_words = wc.map(lambda w: w[1]).reduce(lambda w1, w2: w1 + w2).collect()
freqs = wc.map(lambda w: (w[0], float(w[1])/total_words)).collect()
# freqs = wc.collect()
end = timer()
print(freqs[:10])
print("Time elapsed %f" % (end - start))
