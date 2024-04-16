Because individual `vg` invocations often do only one thing, users will want to string `vg` commands together into larger pipelines and workflows. Often, it is necessary to execute these workflows through a scheduler system on multiple machines, instead of running all the commands interactively on the local machine.

The VG team provides [toil-vg](https://github.com/vgteam/toil-vg) as one potential solution to this problem. However, toil-vg only implements certain workflows. Users may want to run workflows of their own design. This page collects tips from the design of toil-vg that will be useful to users writing other workflows.

## Collecting Error Logs

The `vg` command includes built-in stack trace collection code, which can greatly aid in debugging runtime errors. However, by default, vg writes its stack trace to a temporary file on the local machine. This is desirable behavior for interactive usage, but for batch processing it makes more sense to dump the full stack trace to standard error, so that it can be picked up by the scheduling system and returned to the user. It is recommended that batch workflows set the following environment variable to enable this behavior in `vg`:

```
VG_FULL_TRACEBACK=1
```