import argparse
from types import SimpleNamespace
import os
import yaml

__all__ = ['Config']


class Config(SimpleNamespace):
    """SimpleNamesapce wrapper to support: parse command --config flag"""
    def __init__(self,args=None,recursive_depth:int=-1,is_add:bool = False, conf_file=None):
        """
        Args:
            - args: dict or argparse.Namespace
            - recursive: recursive depth, 0 to disable, -1 to apply fully recursive mode.
            - is_add: True to add attr when calling `get` if attr not exists
        """
        super().__init__()
        self._depth = recursive_depth
        self._add = is_add
        if args:
            args = vars(args) if isinstance(args,argparse.Namespace) else dict(args)
        elif conf_file:
            args = yaml.safe_load(open(conf_file))
        else:
            raise ValueError("Both args and conf_file are None!")

        # parse config file if existing
        if 'config' in args.keys() and os.path.exists(args['config']):
            with open(args['config'],'r') as f:
                yaml_args = yaml.safe_load(f)
                args.update(yaml_args)
 
        for k in args.keys():
            if self._depth != 0:
                # build dict-based config recursively 
                if isinstance(args[k],dict):
                    args[k] = Config(
                        args[k],recursive_depth = -1 if self._depth == -1 else self._depth - 1,is_add=self._add
                    )
                else:
                    continue
        self.__dict__.update(args)
        
    def get(self,key, default_value=None):
        if hasattr(self,key):
            return self.key
        else:
            if self._add:
                self.__setattr__(key,default_value)
            return default_value
    
    def set(self,key,value):
        if isinstance(value,dict) and not self._depth == 0:
            value = Config(value)
        self.__setattr__(key,value)

    def str2bool(self,s):
        if s.lower() in ['0','false']:
            return False
        if s.lower() in ['1', 'true']:
            return True
        else:
            return s

    @staticmethod
    def to_dict(cfg) -> dict:
        """
        cfg: dict or Config
        """
        if isinstance(cfg,Config):
            d = vars(cfg)
            for k,v in d.items():
                if isinstance(v,Config):
                    d[k] = Config.to_dict(d[k])
        else:
            d = cfg
        return d

