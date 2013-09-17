from prj import Module


class RigelModule(Module):
    xml_schema = """
  <schema>
   <entry name="taskid_size" type="int" default="8"/>
   <entry name="signalset_size" type="int" default="8"/>
   <entry name="irqeventid_size" type="int" default="8"/>
   <entry name="prefix" type="c_ident" default="rtos_" />
   <entry name="fatal_error" type="c_ident" />
   <entry name="signals" type="list" auto_index_field="idx">
     <entry name="signal" type="dict">
       <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="tasks" type="list" auto_index_field="idx">
     <entry name="task" type="dict">
      <entry name="entry" type="c_ident" />
      <entry name="name" type="ident" />
      <entry name="stack_size" type="int" />
     </entry>
   </entry>
   <entry name="irq_events" type="list" auto_index_field="idx">
     <entry name="irq_event" type="dict">
      <entry name="name" type="ident" />
      <entry name="task" type="object" group="tasks" />
      <entry name="sig_set" type="int" />
     </entry>
   </entry>
   <entry name="mutexes" type="list" auto_index_field="idx">
     <entry name="mutex" type="dict">
      <entry name="name" type="ident" />
     </entry>
   </entry>
   <entry name="timers" type="list" auto_index_field="idx">
     <entry name="timer" type="dict">
      <entry name="name" type="ident" />
      <entry name="enabled" type="bool" />
      <entry name="reload" type="int" />
      <entry name="error" type="int" default="0" />
      <entry name="task" type="object" group="tasks" optional="true" />
      <entry name="sig_set" type="int" optional="true" />
     </entry>
   </entry>
  </schema>
"""
    files = [
        {'input': 'rtos-rigel.h', 'render': True},
        {'input': 'rtos-rigel.c', 'render': True, 'type': 'c'},
    ]

module = RigelModule()
