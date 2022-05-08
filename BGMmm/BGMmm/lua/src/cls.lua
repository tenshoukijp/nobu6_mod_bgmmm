-----------------------------------------------------------------------------
-- Lua Class Lib
--
-- Author: Robert Ray <louirobert@gmail.com> @ 2011
-- Home Page: http://code.google.com/p/lua-class-lib/
-- Licence: MIT
-- Version: 0.12
-- Note: See home page for document and update!
-----------------------------------------------------------------------------

module("cls", package.seeall)

local function parseName(str)
    local _begin, _end, cls = assert(str:find('%s*([a-zA-Z][a-zA-Z0-9_]*)%s*%:?'))
    if not str:find(':', _end) then
        return cls, {}
    end
    local bases = {}
    while true do
        local base
        _begin, _end, base = str:find('%s*([a-zA-Z][a-zA-Z0-9_]*)%s*%,?', _end + 1)
        if base then
            table.insert(bases, base)
        else
            break
        end
    end
    return cls, bases
end

local function create(t, ...)
    local o = {}
    if t.__init__ then
        t.__init__(o, ...)
    end
    return setmetatable(o, {__index = t, __class__ = t})
end

function class(name)
    local env = getfenv(2)
    local clsName, bases = parseName(name)
    for i, v in ipairs(bases) do
        bases[i] = assert(env[v])   --Replace string name with class table
    end

    return function (t)
        local meta = {__call = create, __bases__ = bases}
        meta.__index = function(nouse, key)
            for _, cls in ipairs(meta.__bases__) do
                local val = cls[key]    --Do a recursive search on each cls
                if val ~= nil then
                    return val
                end
            end
        end
        local clsobj = setmetatable(t, meta)
        env[clsName] = clsobj
        return clsobj
    end
end

function get_class(o)
    local meta = assert(getmetatable(o))
    return meta.__class__
end

function get_base_classes(c)
    local meta = assert(getmetatable(c))
    return meta.__bases__
end

local function up_traverse(cls)
    coroutine.yield(cls)
    local bases = getmetatable(cls).__bases__
    for _, v in ipairs(bases) do
        up_traverse(v)
    end
end

function up_classes(cls)
    return coroutine.wrap(function() up_traverse(cls) end)
end

function is_instance_of(o, c)
    local cls = assert(get_class(o))
    for one in up_classes(cls) do
        if one == c then
            return true
        end
    end
    return false
end

_G.class = class
_G.is_instance_of = is_instance_of



--[[

require 'cls'

module(..., package.seeall)

class 'Person'
{
    __init__ = function(self, name)
        self.name = name
    end;

    say = function(self)
        print('Hello, my name is ' .. self.name .. '.')
        self:saySthElse()
    end;

    saySthElse = function(self)
    end
}

class 'Worker'
{
    __init__ = function(self, id)
        self.id = id
    end;

    showId = function(self)
        print('My worker id is ' .. self.id .. '.')
    end
}

class 'Employee: Person, Worker'
{
    __init__ = function(self, name, salary, id)
        Person.__init__(self, name)
        Worker.__init__(self, id)
        self.salary = salary
    end;

    saySthElse = function(self)
        print('My salary is ' .. self.salary .. '.')
    end
}

]]


--[[
require 'cls_demos'

print '--------------------------------------------'

p = cls_demos.Person('Bob')
p:say()

p2 = cls_demos.Person('David')
p2:say()

print '--------------------------------------------'

e = cls_demos.Employee('Bob', 1000, 1)
e:say()
e:showId()

e2 = cls_demos.Employee('Alice', 10000, 2)
e2:say()
e2:showId()

print '--------------------------------------------'

if is_instance_of(e, cls_demos.Person) then
    print 'e is an instance of Person'
else
    print 'e is not an instance of Person'
end

if is_instance_of(e, cls_demos.Worker) then
    print 'e is an instance of Worker'
else
    print 'e is not an instance of Worker'
end

w = cls_demos.Worker(100)

if is_instance_of(w, cls_demos.Person) then
    print 'w is an instance of Person'
else
    print 'w is not an instance of Person'
end

if is_instance_of(w, cls_demos.Worker) then
    print 'w is an instance of Worker'
else
    print 'w is not an instance of Worker'
end

]]