"""Serial port matching by VID/PID/serial/desc.

Extracted from devtool.py so hardware-map.yml can use descriptive
port matching instead of hard-coded COM port strings.
"""

import serial.tools.list_ports


def match_serial_port(match_cfg, label: str = "serial port") -> str:
    """Resolve a serial port from match criteria.

    Supports two forms:
      - str: direct port name (``"COM38"``) — backward compatible
      - dict: match criteria with any combination of:
          ``vid``, ``pid``, ``serial``, ``desc``, ``port``

    All criteria are AND-ed.  Returns the resolved port name string.
    """
    if isinstance(match_cfg, str):
        return match_cfg

    vid = match_cfg.get("vid")
    pid = match_cfg.get("pid")
    sn = match_cfg.get("serial")
    desc = match_cfg.get("desc")
    location = match_cfg.get("location")
    explicit_port = match_cfg.get("port")

    vid = int(vid) if vid is not None else None
    pid = int(pid) if pid is not None else None

    candidates = []
    for port in serial.tools.list_ports.comports():
        if vid is not None and port.vid != vid:
            continue
        if pid is not None and port.pid != pid:
            continue
        if sn is not None and sn not in (port.serial_number or ""):
            continue
        if desc is not None and desc not in (port.description or ""):
            continue
        if location is not None and location not in (port.location or ""):
            continue
        candidates.append(port.device)

    if len(candidates) == 0:
        raise RuntimeError(f"{label}: no port matching criteria")
    if len(candidates) == 1:
        return candidates[0]
    if explicit_port:
        pn = explicit_port.upper().replace("/", "\\")
        for c in candidates:
            if c.upper().replace("/", "\\") == pn:
                return c
    raise RuntimeError(f"{label}: {len(candidates)} ports match, use 'port' to disambiguate: {candidates}")


# Backward-compat alias
resolve_serial_port = match_serial_port
