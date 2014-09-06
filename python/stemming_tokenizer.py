
# coding: utf-8

## Tokenizer with Stemmer

####### A tokenizer that stem tokenized documents text using nltk porter stemmer

# In[1]:

# alphanumeric tokenizer
from nltk.stem import PorterStemmer
from nltk import RegexpTokenizer
class RawStemmingTokenizer(object):
    def __init__(self):
        self.stemmer = PorterStemmer()
        self.tokenizer = RegexpTokenizer(u'(?u)\\b\\w\\w+\\b')
    def __call__(self, doc):
        return [self.stemmer.stem(tokens.lower()) for tokens in self.tokenizer.tokenize(doc)]


# In[ ]:

# alphabetic tokenizer
from nltk.stem import PorterStemmer
from nltk import RegexpTokenizer
class StemmingTokenizer(object):
    def __init__(self):
        self.stemmer = PorterStemmer()
        self.tokenizer = RegexpTokenizer(u'(?u)\\b[a-z]+\\-*[a-z]+|\\b(?u)\\b[a-z]\\b')
    def __call__(self, doc):
        return [self.stemmer.stem(tokens.lower()) for tokens in self.tokenizer.tokenize(doc)]

