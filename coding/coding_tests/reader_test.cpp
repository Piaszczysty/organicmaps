#include "../../base/SRC_FIRST.hpp"
#include "../../testing/testing.hpp"

#include "reader_test.hpp"

#include "../file_reader.hpp"
#include "../file_writer.hpp"
#include "../buffer_reader.hpp"
#include "../reader_streambuf.hpp"

#include "../../std/fstream.hpp"


namespace
{
  string const kData("Quick brown fox jumps over a lazy dog...");
}

UNIT_TEST(MemReaderSmokeTest)
{
  MemReader memReader(kData.c_str(), kData.size());
  TestReader(memReader);
}

UNIT_TEST(FileReaderSmokeTest)
{
  {
    FileWriter writer("reader_test_tmp.dat");
    writer.Write(&kData[0], kData.size());
  }

  {
    FileReader fileReader("reader_test_tmp.dat");
    TestReader(fileReader);
  }
  FileWriter::DeleteFileX("reader_test_tmp.dat");
}

UNIT_TEST(BufferReaderSmokeTest)
{
  BufferReader r1(&kData[0], kData.size());
  TestReader(r1);

  {
    string const data("BlaBla " + kData);
    FileWriter writer("reader_test_tmp.dat");
    writer.Write(&data[0], data.size());
  }

  BufferReader r2(FileReader("reader_test_tmp.dat"), 7);
  TestReader(r2);
  FileWriter::DeleteFileX("reader_test_tmp.dat");
}

UNIT_TEST(BufferReaderEmptyTest)
{
  MemReader reader(NULL, 0);
  BufferReader bufReader(reader, 0);
  TEST_EQUAL(bufReader.Size(), 0, ());
}

UNIT_TEST(FileReaderNonExistentFileTest)
{
  try
  {
    FileReader reader("skjhfaxniauiuq2bmnszmn093sklsd");
    TEST(false, ("Exception should be thrown!"));
  }
  catch (FileReader::OpenException &)
  {
  }
}

UNIT_TEST(FileReaderReadAsText)
{
  char const fName[] = "zzzuuuuuummmba";
  {
    FileWriter f(fName);
    f.Write(fName, ARRAY_SIZE(fName) - 1);
  }

  {
    string text;
    FileReader(fName).ReadAsString(text);
    TEST_EQUAL(text, fName, ());
  }

  FileWriter::DeleteFileX(fName);
}

UNIT_TEST(SharedMemReader)
{
  SharedMemReader reader1(3);
  TEST_EQUAL(reader1.Size(), 3, ());
  memcpy(reader1.Data(), "123", 3);
  SharedMemReader reader2(reader1);
  TEST_EQUAL(reader2.Size(), 3, ());
  string s1(3, '0'), s2(3, '0');
  reader1.Read(0, &s1[0], 3);
  reader2.Read(0, &s2[0], 3);
  TEST_EQUAL(s1, "123", ());
  TEST_EQUAL(s2, "123", ());
}

UNIT_TEST(ReaderStreamBuf)
{
  string const name = "test.txt";

  {
    WriterStreamBuf buffer(new FileWriter(name));
    ostream s(&buffer);
    s << "hey!" << '\n' << 1 << '\n' << 3.14 << '\n' << 0x0102030405060708ull << std::endl;
  }

  {
    ReaderStreamBuf buffer(new FileReader(name));
    istream s(&buffer);

    std::string str;
    int i;
    double d;
    unsigned long long ull;

    s >> str >> i >> d >> ull;

    TEST_EQUAL(str, "hey!", ());
    TEST_EQUAL(i, 1, ());
    TEST_ALMOST_EQUAL(d, 3.14, ());
    TEST_EQUAL(ull, 0x0102030405060708ull, ());
  }

  FileWriter::DeleteFileX(name);
}
