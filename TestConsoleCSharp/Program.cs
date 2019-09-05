using System;
using System.Diagnostics;
using SevenZip.CLI;


namespace TestConsoleCSharp
{
    class Program
    {
        static void Main(string[] args)
        {
            SevenZipCompressor compressor = new SevenZipCompressor(@"PATH_TO_SEVENZIPDLL_\7z.dll",
                                                    @"D:\Temp\Test\test.xml");

            compressor.SetCompressionFormat(CompressionFormat.GZip);
            compressor.SetCompressionLevel(CompressionLevel.Fast);

            compressor.AddFile(@"D:\Temp\Test\test.xml");

            Stopwatch watch = new Stopwatch();
            watch.Start();
            compressor.DoCompress();
            watch.Stop();

            Console.WriteLine($"Compression tooks {watch.Elapsed:g}");


            SevenZipExtractor extractor = new SevenZipExtractor(@"PATH_TO_SEVENZIPDLL_\7z.dll",
                                                                 @"D:\Temp\Test\test.xml.gz");

            extractor.SetCompressionFormat(CompressionFormat.GZip);

            watch.Start();
            extractor.ExtractArchive(@"D:\Temp\Test\out");
            watch.Stop();

            Console.WriteLine($"Extraction tooks {watch.Elapsed:g}");

            Console.ReadKey();
        }
    }
}
