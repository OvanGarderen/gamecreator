#!/bin/python2

class structwrap:
    def __init__(self,desc):
        self.type = desc[:desc.index('{')].replace('struct','').strip()
        desc = desc[desc.index('{')+1:]
        desc = desc[:desc.index('}')]
        desc = map(lambda x: x.strip(),filter(lambda x: x, desc.split(';')))
        self.variables = []
        for var in desc:
            var = var.split(' ')
            self.variables.append((' '.join(var[:-1]),var[-1]))

    def internal_accessor(self,var):
        if '*' in var[0]:
            var = (var[0].split(' ')[0],var[1])
            return """if(!strcmp(symbol,"%s"))\n\t\treturn Value_point_%s(this->%s);\n\t""" % (var[1],var[0].lower(),var[1])
        elif var[0].split(' ')[0] == var[0]:
            return """if(!strcmp(symbol,"%s"))\n\t\treturn Value_point_%s(&(this->%s));\n\t""" % (var[1],var[0].lower(),var[1])
        else:
            return ""
            
    def get_accessor(self):
        ret = """Value * type_%s_resolve(Value *v, char * symbol) {\n\t%s * this = v->get;\n\t""" % (self.type.lower(),self.type)
        ret += ' else '.join(filter(lambda x: x,map(self.internal_accessor,self.variables)))
        ret += """return Value_wrap_none();\n}\n"""
        return ret
    
if __name__ == "__main__":
    wrap = structwrap("struct Resource {int a; float b; int * ap}")
    print wrap.type
    print wrap.get_accessor()
