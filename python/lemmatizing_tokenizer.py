
# coding: utf-8

## Tokenizer with Lemmatizer

####### A tokenizer that lemmatize tokenized documents text using nltk wordnet lemmatizer

# In[1]:

from nltk import RegexpTokenizer
from nltk.stem import WordNetLemmatizer
class LemmaTokenizer(object):
    def __init__(self):
        self.tokenizer = RegexpTokenizer(u'(?u)\\b[a-z]+\\-*[a-z]+|\\b(?u)\\b[a-z]\\b')
        self.wnl = WordNetLemmatizer()
    def __call__(self, doc):
        return [self.wnl.lemmatize(t) for t in self.tokenizer.tokenize(doc)]


# In[ ]:



