from pyparsing import *

TRUE = Keyword("true").setParseAction( replaceWith(True) )
FALSE = Keyword("false").setParseAction( replaceWith(False) )
NULL = Keyword("null").setParseAction( replaceWith(None) )

jsonString = dblQuotedString.setParseAction( removeQuotes )
jsonNumber = Combine( Optional('-') + ( '0' | Word('123456789',nums) ) +
                    Optional( '.' + Word(nums) ) +
                    Optional( Word('eE',exact=1) + Word(nums+'+-',nums) ) )

jsonObject = Forward()
jsonValue = Forward()
jsonElements = delimitedList( jsonValue )
jsonArray = Group(Suppress('(') + Optional(jsonElements) + Suppress(')') )
jsonValue << ( jsonString | jsonNumber | jsonObject | jsonArray | TRUE | FALSE | NULL )

jsonName = Word(alphanums+"_-.")
memberDef = dictOf( jsonName + ( Suppress(':') | Suppress('=') ), jsonValue )
jsonMembers = Dict( memberDef )
jsonObject << Dict( Suppress('{') + Optional(jsonMembers) + Suppress('}') )

jsonComment = cppStyleComment 
jsonObject.ignore( jsonComment )

def convertNumbers(s,l,toks):
    n = toks[0]
    try:
        return int(n)
    except ValueError, ve:
        return float(n)
        
jsonNumber.setParseAction( convertNumbers )

def make_a_fucking_dictionary(result):
    try:
        #len might fail
        if len(result) == len(result.keys()) and len(result) > 0:
            #we have a dictionary
            d = dict()
            for key in result.keys():
                d[key] = make_a_fucking_dictionary(result[key])
            return d
        else:
            return map(make_a_fucking_dictionary,result)
    except:
        return result

def print_back(d,indent):
    ret = ""
    if isinstance(d,dict):
        #we have a dictionary
        ret += '{\n'
        indent = indent + 1
        for key in d:
            ret+= reduce(lambda x,y: x+'\t',range(indent),"")
            ret+= key + ' : ' + print_back(d[key],indent) + '\n'
        indent = indent - 1
        ret+= reduce(lambda x,y: x+'\t',range(indent),"")
        ret += '}'
    elif isinstance(d,list):
        ret += '(\n'
        indent = indent + 1
        for entry in d:
            ret+= reduce(lambda x,y: x+'\t',range(indent),"")
            ret+= print_back(entry,indent) + ',\n'
        ret = ret[:-2] + '\n'
        indent = indent - 1
        ret+= reduce(lambda x,y: x+'\t',range(indent),"")
        ret += ')'
    elif isinstance(d,tuple):
        d = list(d)
        ret += '('
        ret+= ', '.join(map(lambda x: print_back(x,indent),d))
        ret += ')'
    elif isinstance(d,str):
        ret += '"'+d+'"'
    else:
        ret += str(d) 
    return ret    

def dict_for_file(d):
    return print_back(d,-1)[2:-2]


def parseObj(string):
    return make_a_fucking_dictionary(jsonObject.parseString(string))


    


