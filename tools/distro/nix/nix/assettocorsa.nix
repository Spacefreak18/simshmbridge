{ buildEnv
, simshmbridge-assettocorsa
, createsimshm-assettocorsa
,
}:
buildEnv {
  name = "simshm-assettocorsa";

  paths = [
    simshmbridge-assettocorsa
    createsimshm-assettocorsa
  ];
}
