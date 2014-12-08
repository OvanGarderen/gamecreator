#variable/object names may not contain spaces or the special character :

def resplit(tokens, str=' '):
    import re
    return reduce(list.__add__,map(lambda x: [x] if x and x[0]=='"' else re.split('('+str+')',x,), tokens),[])

def tokenize(string):
    import re
    tokens = re.split(r'(\".+?\")',string)
    tokens = resplit(resplit(tokens,'{'),'}')
    tokens = resplit(resplit(tokens,r'\('),r'\)')
    tokens = resplit(tokens,',')
    tokens = resplit(resplit(tokens,' '),'\n')
    tokens = resplit(resplit(tokens,'='),':')
    tokens = filter(lambda x: x and not x.isspace(),tokens)
    print tokens
    return tokens

class JsonObject:
    def __init__(self,tokens):
        self.entries = dict()
        name = None
        sets = False
        for token in tokens:
            if name == None:
                name = str(token)
            elif not sets:
                if token in [':','=']:
                    sets = True
                else:
                    print token
                    raise ParseError
            else:
                self.entries[name] = token
                name = None
                sets = False
            
    def __contains__(self,key):
        return key in self.entries.keys()

    def get(self,key, default=None):
        return self.entries[key] if key in self.entries.keys() else default

    def keys(self):
        return self.entries.keys()

    def __getitem__(self,key):
        return self.entries[key]
    def __setitem__(self,key,val):
        self.entries[key] = val

    def renderasstr(self,thing):
        if isinstance(thing,str):
            return '"'+str(thing)+'"'
        else:
            return str(thing)

    def __repr__(self):
        return '{\n' + '\n'.join([key + ' : ' + self.renderasstr(self.entries[key]) for key in self.keys()]) + '}\n'

class JsonList:
    def __init__(self,tokens):
        self.entries = []
        comma = True
        print '>>>>>',tokens
        for token in tokens:
            print token
            if comma:
                if token == ',':
                    raise ParseError
                else:
                    self.entries.append(token)
                comma = False
            else:
                comma = True
                if token != ',':

                    raise ParseError

    def __getitem__(self,key):
        return self.entries[key]
    def __setitem__(self,key,val):
        self.entries[key] = val

    def __repr__(self):
        return '(' + ','.join(map(str,self.entries)) + ')'

def evalobj(obj):
    if obj[0] == '"' and obj[-1] == '"':
        return obj[1:-1]
    if '.' in str(obj):
        try:
            return float(obj)
        except:
            pass
    else:
        try:
            return int(obj)
        except:
            pass
    return obj

def matchbraces(tokens):
    braces = 0
    braces = 0
    btype = 'none'
    objs = []
    acc = []
    for token in tokens:
        #print token,btype,braces, '>>', objs
        if token == '(':
            if btype == 'none':
                btype = 'square'
                braces += 1
            elif btype == 'square':
                braces += 1
                acc.append(token)
            else:
                acc.append(token)
        elif token == '{':
            if btype == 'none':
                btype = 'curly'
                braces += 1
            elif btype == 'curly':
                braces += 1
                acc.append(token)
            else:
                acc.append(token)
        elif token == ')'and btype == 'square':
            braces -= 1
            if braces == 0:
                objs.append(JsonList(matchbraces(acc)))
            else:
                acc.append(token)
                    
        elif token == '}' and btype == 'curly':
            braces -= 1
            if braces == 0:
                objs.append(JsonObject(matchbraces(acc)))
                acc = []
            else:
                acc.append(token)
        else:
            if btype == 'none':
                objs.append(evalobj(token))
            else:
                acc.append(token)
    return objs
    

class ParseError(Exception):
    pass

def parseObj(string):
    return JsonObject(matchbraces(tokenize(string)))

def parseList(string):
    return JsonList(matchbraces(tokenize(string)))
