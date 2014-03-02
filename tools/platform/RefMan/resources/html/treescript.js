<!-- // Hide script from old browsers

    function toggleFolder(id, imageNode)
    {
      var folder = document.getElementById(id);
      var l = imageNode.src.length;
      if (imageNode.src.substring(l-27,l)=="images/ftv2folderclosed.png" ||
          imageNode.src.substring(l-25,l)=="images/ftv2folderopen.png")
      {
        imageNode = imageNode.previousSibling;
        l = imageNode.src.length;
      }
      if (folder == null)
      {
      }
      else if (folder.style.display == "block")
      {
        if (imageNode != null)
        {
          imageNode.nextSibling.src = "images/ftv2folderclosed.png";
          if (imageNode.src.substring(l-20,l) == "images/ftv2mnode.png")
          {
            imageNode.src = "images/ftv2pnode.png";
          }
          else if (imageNode.src.substring(l-24,l) == "images/ftv2mlastnode.png")
          {
            imageNode.src = "images/ftv2plastnode.png";
          }
        }
        folder.style.display = "none";
      }
      else
      {
        if (imageNode != null)
        {
          imageNode.nextSibling.src = "images/ftv2folderopen.png";
          if (imageNode.src.substring(l-20,l) == "images/ftv2pnode.png")
          {
            imageNode.src = "images/ftv2mnode.png";
          }
          else if (imageNode.src.substring(l-24,l) == "images/ftv2plastnode.png")
          {
            imageNode.src = "images/ftv2mlastnode.png";
          }
        }
        folder.style.display = "block";
      }
    }

    // End script hiding -->