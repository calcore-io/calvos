'''
Created on Dec 20, 2020

@author: Carlos Calvillo
'''

TYPE_SCALAR = 0
TYPE_ENUM = 1
TYPE_STRING = 2

class Parameter():
    
    def __init__(self, id, value = None, default = None, \
                 name = "", description = ""):
        self.id = id
        self.value = value
        self.default = default
        self.name = name
        self.desc = description
        
        self.category = None
        
        self.enumeration = {}
        self.type = TYPE_SCALAR
        self.multiple = False
        
        
        self.min = None
        self.max = None
        self.offset = None
        self.resolution = None
        
    def is_valid_value(self, value = None):
        """ validate if the given value is within the defined enum. """
        return_value = False
        
        if value is None:
            # Validate current parameter value
            if self.value in self.enumeration:
                return_value = True
        else:
            # Validate passed value
            if value in self.enumeration:
                return_value = True 
        
        return return_value

class ParamCategory():
    
    def __init__(self, id, name = "", description = ""):
        self.id = id
        self.name = name
        self.desc = description