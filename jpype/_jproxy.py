# *****************************************************************************
#   Copyright 2004-2008 Steve Menard
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# *****************************************************************************
import _jpype

__all__ = ["JProxy", "JImplements"]


# FIXME the java.lang.method we are overriding should be passes to the lookup function
# so we can properly handle name mangling on the override.

def _checkInterfaceOverrides(interfaces, overrides):
    # Verify all methods are overriden
    for interface in interfaces:
        for method in interface.class_.getMethods():
            if method.getModifiers() & 1024 == 0:
                continue
            if not str(method.getName()) in overrides:
                raise NotImplementedError("Interface '%s' requires method '%s' to be implemented." % (
                    interface.class_.getName(), method.getName()))


def _createJProxy(cls, *intf, **kwargs):
    """ (internal) Create a proxy from a python class with
    @JOverride notation on methods.
    """
    # TODO: When Python2 is dropped these kwargs can become keyword-only kwargs
    deferred = bool(kwargs.pop('deferred', False))
    if kwargs:
        raise TypeError('Invalid keywords: {}'.format(
            ','.join([str(kwarg) for kwarg in kwargs])))

    def _classOverrides(cls):
        # Find all class defined overrides
        overrides = {}
        for k, v in cls.__dict__.items():
            try:
                attr = object.__getattribute__(v, "__joverride__")
                overrides[k] = (v, attr)
            except AttributeError:
                pass
        return overrides

    def _prepareInterfaces(cls, intf):
        # Convert the interfaces list
        actualIntf = _convertInterfaces(intf)
        overrides = _classOverrides(cls)
        _checkInterfaceOverrides(actualIntf, overrides)
        return actualIntf

    def new(tp, *args, **kwargs):
        # Attach a __jpype_interfaces__ attribute to this class if
        # one doesn't already exist.
        if getattr(tp, "__jpype_interfaces__", None) is None:
            tp.__jpype_interfaces__ = _prepareInterfaces(cls, intf)
        self = _jpype._JProxy.__new__(tp, None, tp.__jpype_interfaces__)
        tp.__init__(self, *args, **kwargs)
        return self

    members = {'__new__': new}
    if not deferred:
        members['__jpype_interfaces__'] = _prepareInterfaces(cls, intf)

    # Return the augmented class
    return type("proxy.%s" % cls.__name__, (cls, _jpype._JProxy), members)


def JImplements(*interfaces, **kwargs):
    """ Annotation for creating a new proxy that implements one or more
    Java interfaces.

    This annotation is placed on an ordinary Python class.  The annotation
    requires a list of interfaces.  It must implement all of the java
    methods for each of the interfaces.  Each implemented method
    should have a @JOverride annotation.

    Args:
      interfaces (str*,JClass*): Strings or JClasses for each Java interface
        this proxy is to implement.

    Kwargs:
      deferred (bool):
        Whether to validate the override and attach the Proxy at
        import time (requires JVM to be running), or at instance instantiation.
        There is no performance cost to doing the work at instance
        instantiation as the proxy is attached to the class once and is
        subsequently cached on the class itself.

    Example:

      .. code-block:: python

          @JImplement("java.lang.Runnable")
          class MyImpl(object):
             @JOverride
             def run(self, arg):
               pass

          @JImplement("org.my.Interface1", "org.my.Interface2")
          class MyImpl(object):
             @JOverride
             def method(self, arg):
               pass

    """
    def JProxyCreator(cls):
        return _createJProxy(cls, *interfaces, **kwargs)
    return JProxyCreator


def _convertInterfaces(intf):
    """ (internal) Convert a list of interface names into
    a list of interfaces suitable for a proxy.
    """
    # Flatten the list
    intflist = []
    for item in intf:
        if isinstance(item, str) or not hasattr(item, '__iter__'):
            intflist.append(item)
        else:
            intflist.extend(item)

    # Look up the classes if given as a string
    actualIntf = set()
    for item in intflist:
        if isinstance(item, str):
            actualIntf.add(_jpype.JClass(item))
        else:
            actualIntf.add(item)

    # Check that all are interfaces
    if not actualIntf:
        raise TypeError("At least one Java interface must be specified")

    for cls in actualIntf:
        # If it isn't a JClass, then it cannot be a Java interface
        if not isinstance(cls, _jpype.JClass):
            raise TypeError("'%s' is not a Java interface" %
                            type(cls).__name__)
        # Java concrete and abstract classes cannot be proxied
        if not issubclass(cls, _jpype.JInterface):
            raise TypeError("'%s' is not a Java interface" % cls.__name__)

    return tuple(actualIntf)


class _JFromDict(object):
    def __init__(self, dict):
        self.dict = dict

    def __getattribute__(self, name):
        try:
            return object.__getattribute__(self, 'dict')[name]
        except KeyError:
            pass
        raise AttributeError("attribute not found")


class JProxy(_jpype._JProxy):
    """ Define a proxy for a Java interface.

    This is an older style JPype proxy interface that uses either a
    dictionary or an object instance to implement methods defined
    in java.  The python object can be held by java and its lifespan
    will continue as long as java holds a reference to the object
    instance.  New code should use ``@JImplements`` annotation as
    it will support improved type safety and error handling.

    Name lookups can either made using a dictionary or an object
    instance.  One of these two options must be specified.

    Args:
        intf: either a single interface or a list of java interfaces.
            The interfaces can either be defined by strings or
            JClass instance.  Only interfaces may be used in a
            proxy,
        dict (dict[string, callable], optional): specifies a dictionary
            containing the methods to be called when executing the
            java interface methods.
        inst (object, optional): specifies an object with methods
            whose names matches the java interfaces methods.
    """
    def __new__(cls, intf, dict=None, inst=None, convert=False):
        # Convert the interfaces
        actualIntf = _convertInterfaces([intf])

        # Verify that one of the options has been selected
        if dict is not None and inst is not None:
            raise TypeError("Specify only one of dict and inst")

        if dict is not None:
            return _jpype._JProxy(_JFromDict(dict), actualIntf, convert)

        if inst is not None:
            return _jpype._JProxy.__new__(cls, inst, actualIntf, convert)

        raise TypeError("a dict or inst must be specified")

    @staticmethod
    def unwrap(obj):
        if not isinstance(obj, _jpype._JProxy):
            return obj
        return obj.__javainst__
