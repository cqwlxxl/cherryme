-- phpMyAdmin SQL Dump
-- version 5.2.0
-- https://www.phpmyadmin.net/
--
-- 主机： 127.0.0.1
-- 服务器版本： 10.4.25-MariaDB
-- PHP 版本： 7.4.30

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- 数据库： `cherryme`
--

-- --------------------------------------------------------

--
-- 表的结构 `anime_episode`
--

CREATE TABLE `anime_episode` (
  `eid` int(11) NOT NULL,
  `aid` int(11) NOT NULL,
  `sid` int(11) NOT NULL,
  `episode` tinytext NOT NULL,
  `title` text NOT NULL,
  `see` tinyint(4) NOT NULL DEFAULT 0,
  `tag1` tinyint(4) NOT NULL DEFAULT 0,
  `tag2` tinyint(4) NOT NULL DEFAULT 0,
  `tag3` tinyint(4) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- --------------------------------------------------------

--
-- 表的结构 `anime_ip`
--

CREATE TABLE `anime_ip` (
  `aid` int(11) NOT NULL,
  `name` text NOT NULL,
  `keywords` tinytext NOT NULL,
  `see` tinyint(4) NOT NULL DEFAULT 0,
  `see_season` int(11) NOT NULL DEFAULT 0,
  `total_season` int(11) NOT NULL DEFAULT 0,
  `zhuifan` tinyint(4) NOT NULL DEFAULT 0,
  `collect` tinyint(4) NOT NULL DEFAULT 0,
  `point` tinyint(4) NOT NULL DEFAULT 0,
  `display` tinyint(4) DEFAULT 0,
  `tag1` tinyint(4) NOT NULL DEFAULT 0,
  `tag2` tinyint(4) NOT NULL DEFAULT 0,
  `tag3` tinyint(4) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

-- --------------------------------------------------------

--
-- 表的结构 `anime_recent`
--

CREATE TABLE `anime_recent` (
  `id` int(11) NOT NULL,
  `aid` int(11) NOT NULL,
  `sid` int(11) NOT NULL,
  `name` text NOT NULL,
  `display` tinyint(4) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `anime_season`
--

CREATE TABLE `anime_season` (
  `sid` int(11) NOT NULL,
  `aid` int(11) NOT NULL,
  `name` text NOT NULL,
  `release_date` date NOT NULL DEFAULT '1970-01-01',
  `see` tinyint(4) NOT NULL DEFAULT 0,
  `see_episode` int(11) NOT NULL DEFAULT 0,
  `total_episode` int(11) NOT NULL DEFAULT 0,
  `collect` tinyint(4) NOT NULL DEFAULT 0,
  `point` tinyint(4) NOT NULL DEFAULT 0,
  `display` tinyint(4) NOT NULL DEFAULT 0,
  `tag1` tinyint(4) NOT NULL DEFAULT 0,
  `tag2` tinyint(4) NOT NULL DEFAULT 0,
  `tag3` tinyint(4) NOT NULL DEFAULT 0
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC;

--
-- 转储表的索引
--

--
-- 表的索引 `anime_episode`
--
ALTER TABLE `anime_episode`
  ADD PRIMARY KEY (`eid`) USING BTREE;

--
-- 表的索引 `anime_ip`
--
ALTER TABLE `anime_ip`
  ADD PRIMARY KEY (`aid`) USING BTREE;

--
-- 表的索引 `anime_recent`
--
ALTER TABLE `anime_recent`
  ADD PRIMARY KEY (`id`);

--
-- 表的索引 `anime_season`
--
ALTER TABLE `anime_season`
  ADD PRIMARY KEY (`sid`) USING BTREE;

--
-- 在导出的表使用AUTO_INCREMENT
--

--
-- 使用表AUTO_INCREMENT `anime_episode`
--
ALTER TABLE `anime_episode`
  MODIFY `eid` int(11) NOT NULL AUTO_INCREMENT;

--
-- 使用表AUTO_INCREMENT `anime_ip`
--
ALTER TABLE `anime_ip`
  MODIFY `aid` int(11) NOT NULL AUTO_INCREMENT;

--
-- 使用表AUTO_INCREMENT `anime_recent`
--
ALTER TABLE `anime_recent`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;

--
-- 使用表AUTO_INCREMENT `anime_season`
--
ALTER TABLE `anime_season`
  MODIFY `sid` int(11) NOT NULL AUTO_INCREMENT;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
