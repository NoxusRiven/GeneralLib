#pragma once

struct StorageArgs
{
    size_t entity;
};

class Storage
{
private:
    StorageArgs _fields;

public:
    Storage(const StorageArgs& args) : _fields(args) {}
};