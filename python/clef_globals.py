
# coding: utf-8

## CLEF-IP2010 global definitions

# In[ ]:

from sets import Set
max_labels = 4 # use only top 4 probabilities labels as maximum labels per patent is 4
min_df = 2
min_tf = 3
test_set_size = 0.20
max_chi_square_terms = 10000
db_path = '/home/wshalaby/work/patents/patents-similarity/data/CLEF/03-patents-with-5-fixes.db'
#db_path = '/home/wshalaby/work/patents/patents-similarity/data/CLEF/04-patents-full-revised.db'
#db_path = '/root/Desktop/data-and-indices/patents-data/CLEF-IP/2010/08-clef-patents.db'
train_or_test_values = Set(['train','test','both'])
vocabulary_src_values = Set(['abstract','description','claims'])

