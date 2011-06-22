import Options
from os import unlink, symlink, popen
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
  opt.tool_options("compiler_cxx")

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")
  conf.check_cxx(lib='GeoIP', mandatory=True, uselib_store='GeoIP')

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")
  obj.target = "node-geoip-sync"
  obj.source = "src/node-geoip-sync.cpp"
  obj.uselib = 'GeoIP'

def shutdown():
  if Options.commands['clean']:
    if exists('node-geoip-sync.node'): unlink('node-geoip-sync.node')
  else:
    if exists('build/default/node-geoip-sync.node') and not exists('node-geoip-sync.node'):
      symlink('build/default/node-geoip-sync.node', 'node-geoip-sync.node')
