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
  obj.target = "geoip-sync"
  obj.source = "src/geoip-sync.cpp"
  obj.uselib = 'GeoIP'

def shutdown():
  if Options.commands['clean']:
    if exists('geoip-sync.node'): unlink('geoip-sync.node')
  else:
    if exists('build/default/geoip-sync.node') and not exists('geoip-sync.node'):
      symlink('build/default/geoip-sync.node', 'geoip-sync.node')
