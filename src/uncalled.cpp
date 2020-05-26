#include <iostream>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "self_align_ref.hpp"
#include "mapper.hpp"
#include "map_pool.hpp"
#include "realtime_pool.hpp"
#include "chunk.hpp"
#include "read_buffer.hpp"

namespace py = pybind11;
using namespace pybind11::literals;

PYBIND11_MODULE(mapping, m) {
    m.doc() = "UNCALLED";

    py::class_<Conf> conf(m, "Conf");

    conf.def(py::init<const std::string &>())
        .def("load_conf", &Conf::load_conf);
    Conf::add_pybind_vars(conf);

    py::class_<Paf> paf(m, "Paf");
    paf.def(py::init())
       .def("print_paf", &Paf::print_paf)
       .def("is_mapped", &Paf::is_mapped)
       .def("is_ended", &Paf::is_ended)
       .def("set_int", &Paf::set_int)
       .def("set_float", &Paf::set_float)
       .def("set_str", &Paf::set_str);

    py::enum_<Paf::Tag>(paf, "Tag")
        .value("MAP_TIME", Paf::Tag::MAP_TIME)
        .value("EJECT", Paf::Tag::EJECT)
        .value("IN_SCAN", Paf::Tag::IN_SCAN)
        .value("ENDED", Paf::Tag::ENDED)
        .value("KEEP", Paf::Tag::KEEP)
        .export_values();

    //.def(py::init<const std::string &,const std::string &,u32>())

    py::class_<MapPool>(m, "MapPool")
        .def(py::init<Conf &>())
        .def("update", &MapPool::update)
        .def("add_fast5", &MapPool::add_fast5)
        .def("running", &MapPool::running)
        .def("stop", &MapPool::stop); 

    py::class_<RealtimePool>(m, "RealtimePool")
        .def(py::init<Conf &>()) 
        .def("update", &RealtimePool::update)
        .def("all_finished", &RealtimePool::all_finished)
        .def("stop_all", &RealtimePool::stop_all)
        .def("add_chunk", &RealtimePool::add_chunk);
    
    py::enum_<RealtimeParams::Mode>(m, "RealtimeMode")
        .value("DEPLETE", RealtimeParams::Mode::DEPLETE)
        .value("ENRICH", RealtimeParams::Mode::ENRICH)
        .export_values();

    py::enum_<RealtimeParams::ActiveChs>(m, "ActiveChs")
        .value("FULL", RealtimeParams::ActiveChs::FULL)
        .value("EVEN", RealtimeParams::ActiveChs::EVEN)
        .value("ODD", RealtimeParams::ActiveChs::ODD)
        .export_values();

    py::class_<Chunk>(m, "Chunk")
        .def(py::init<const std::string &, //id, 
                      u16, //channel
                      u32, //number, 
                      u64, //chunk_start, 
                      const std::string &, //dtype
                      const std::string & //raw_str
                     >())
        .def(py::init<const std::string&, //_id, 
                      u32, //number 
                      u16, //channel
                      u64, //chunk_start_sample, 
                      const std::vector<float> &, //raw_data, 
                      u32, //raw_st
                      u32  //raw_len
                     >())
        .def("get_channel", &Chunk::get_channel)
        .def("get_number", &Chunk::get_number)
        .def("empty", &Chunk::empty)
        .def("print", &Chunk::print)
        .def("size", &Chunk::size);
    
    m.def("self_align", &self_align);

    //py::class_< PoreModel<KLEN> >(m, "PoreModel")
    //    .def(py::init<const std::string &, bool>())
    //    .def("match_prob", (float (PoreModel::*) (f &PoreModel<KLEN>::match_prob);

    py::class_<ReadBuffer>(m, "ReadBuffer")
        .def("empty",        &ReadBuffer::empty)
        .def("size",         &ReadBuffer::size)
        .def("get_id",       &ReadBuffer::get_id)
        .def("get_start",    &ReadBuffer::get_start)
        .def("get_end",      &ReadBuffer::get_end)
        .def("get_duration", &ReadBuffer::get_duration)
        .def("get_channel",  &ReadBuffer::get_channel)
        .def("get_raw",      &ReadBuffer::get_raw);

    py::class_<Fast5Reader>(m, "Fast5Reader")
        .def(py::init<u32, u32>())
        .def("add_fast5",       &Fast5Reader::add_fast5) 
        .def("load_fast5_list", &Fast5Reader::load_fast5_list)
        .def("add_read",        &Fast5Reader::add_read)
        .def("load_read_list",  &Fast5Reader::load_read_list)
        .def("pop_read",        &Fast5Reader::pop_read)
        .def("buffer_size",     &Fast5Reader::buffer_size)
        .def("fill_buffer",     &Fast5Reader::fill_buffer)
        .def("all_buffered",    &Fast5Reader::all_buffered)
        .def("empty",           &Fast5Reader::empty);

    py::class_<Event>(m, "Event")
        .def_readwrite("mean", &Event::mean)
        .def_readwrite("stdv", &Event::stdv)
        .def_readwrite("start", &Event::start)
        .def_readwrite("length", &Event::length);


    py::class_<EventDetector>(m, "EventDetector")
        .def(py::init())
        .def("add_sample", &EventDetector::add_sample)
        .def("get_event",  &EventDetector::get_event)
        .def("get_events", &EventDetector::get_events);

    //py::class_<EventDetector>(m, "EventDetector")
    //    .def(py::init<EventParams>());

    //
    //static EventParams const event_detection_defaults = {
    //    .window_length1 = 3,
    //    .window_length2 = 6,
    //    .threshold1     = 1.4f,
    //    .threshold2     = 1.1f,
    //    .peak_height    = 0.2f,
    //    .min_mean       = 30,
    //    .max_mean       = 150
    //};

    /////////////////////////////

    /*
    py::class_< DTW<float, u16, PoreModel> >(m, "DTW")
        .def(py::init<const std::vector<float>, 
                      const std::vector<u16>,
                      const KmerModel &,
                      SubSeqDTW, float, float, float> ())
        .def("score", &DTW<float, u16, KmerModel>::score)
        .def("mean_score", &DTW<float, u16, KmerModel>::mean_score)
        .def("get_path", &DTW<float, u16, KmerModel>::get_path)
        .def("print_path", &DTW<float, u16, KmerModel>::print_path);
    
    py::class_<EventParams>(m, "EventParams")
        .def(py::init());
    
    py::class_<EventDetector>(m, "EventDetector")
        .def(py::init<EventParams>())
        .def("reset", &EventDetector::reset)
        .def("add_samples", &EventDetector::add_samples);

    py::class_<ReadBuffer>(m, "ReadBuffer")
        .def("get_raw", &ReadBuffer::get_raw)
        .def("get_id", &ReadBuffer::get_id)
        .def("get_raw_length", &ReadBuffer::get_raw_length);

    py::class_<KmerModel>(m, "KmerModel")
        .def(py::init<const std::string &>())
        .def("normalize", &KmerModel::normalize)
        .def("str_to_kmers", &KmerModel::str_to_kmers);
    
    py::enum_<SubSeqDTW>(m, "SubSeqDTW")
        .value("NONE", SubSeqDTW::NONE)
        .value("COL", SubSeqDTW::ROW)
        .value("ROW", SubSeqDTW::ROW)
        .export_values();
    */
}

