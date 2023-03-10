#include <CPetBasic.h>

int
main(int argc, char **argv)
{
  using FileNames = std::vector<std::string>;

  FileNames fileNames;

  bool list      = false;
  bool highlight = false;
  bool run       = false;
  bool loop      = false;
  bool raw       = false;
  bool debug     = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "list"     ) list      = true;
      else if (arg == "highlight") highlight = true;
      else if (arg == "run"      ) run       = true;
      else if (arg == "loop"     ) loop      = true;
      else if (arg == "raw"      ) raw       = true;
      else if (arg == "debug"    ) debug     = true;
    }
    else
      fileNames.push_back(argv[i]);
  }

  CPetBasic basic;

  basic.setListHighlight(highlight);

  if (raw)
    basic.setRaw(true);

  basic.setDebug(debug);

  for (const auto &fileName : fileNames)
    basic.loadFile(fileName);

  if (list)
    basic.list();

  if (run)
    basic.run();

  if (loop)
    basic.loop();

  return 0;
}
